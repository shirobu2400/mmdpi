
#include "mmdpi_bone.h"


//	matrix 分だけ変換する
int mmdpiBone::set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix )
{
	if( mmdpiBone::bone_num <= bone_index )
		return -1;
	bone[ bone_index ].delta_matrix = bone[ bone_index ].delta_matrix * matrix;
	return 0;
}

//	ボーンローカルマテリックス生成
void mmdpiBone::make_local_matrix( void )
{
	for( uint i = 0; i < mmdpiBone::bone_num; i ++ )
		bone[ i ].local_matrix = bone[ i ].offset_mat * bone[ i ].matrix;
}

//	マトリックス生成
int mmdpiBone::global_matrix( void )
{
	return make_matrix( &bone[ 0 ], 0x00 );
}

//	グローバル行列生成
int mmdpiBone::make_matrix( MMDPI_BONE_INFO_PTR my_bone, const mmdpiMatrix* offset )
{
	if( my_bone == 0x00 )
		return -1;

	if( offset )
		my_bone->matrix = my_bone->bone_mat * ( *offset ) * my_bone->delta_matrix;
	else
		my_bone->matrix = my_bone->bone_mat * my_bone->delta_matrix;

	if( my_bone->first_child )
		make_matrix( my_bone->first_child, &my_bone->matrix );
	if( my_bone->sibling )
		make_matrix( my_bone->sibling, offset );

	return 0;
}

//	PMX
mmdpiMatrix mmdpiBone::make_global_matrix( int index )
{
	if( bone[ index ].parent == 0x00 )
		return bone[ index ].bone_mat * bone[ index ].delta_matrix;
	return bone[ index ].bone_mat * bone[ index ].parent->matrix * bone[ index ].delta_matrix;
}

//	ボーンのモデル上のローカル座標系の取得
mmdpiMatrix mmdpiBone::get_global_matrix( MMDPI_BONE_INFO_PTR bone )	
{
	if( bone->parent )	
	{
		mmdpiMatrix	parent_matrix = get_global_matrix( bone->parent );
		return bone->bone_mat * parent_matrix;
	}
	return bone->bone_mat;
}

void mmdpiBone::refresh_bone_mat( void )
{
	//	初期化
	for( uint i = 0; i < bone_num; i ++ )
	{
		bone[ i ].bone_mat = bone[ i ].init_mat;
		bone[ i ].delta_matrix.initialize();
	}
}

void mmdpiBone::init_mat_calc( MMDPI_BONE_INFO_PTR my_bone, mmdpiMatrix* offset )
{
	if( my_bone->first_child )	
		init_mat_calc( my_bone->first_child, &my_bone->offset_mat );
	if( my_bone->sibling )
		init_mat_calc( my_bone->sibling, offset );

	if( offset )
		my_bone->init_mat = ( *offset ) * my_bone->init_mat;
}

mmdpiMatrix mmdpiBone::init_mat_calc_bottom( MMDPI_BONE_INFO_PTR now_bone )
{
	if( now_bone->parent )
		return now_bone->parent->offset_mat * now_bone->init_mat;
	return now_bone->init_mat;
}

//	物理演算再生
int mmdpiBone::advance_time_physical( int fps )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( bullet_flag <= 0 )
		return 0;
	
	float	frametime = getFrameTime( fps );
	if( frametime > 10 )
		frametime = 10;

	for( dword i = 0; i < rigidbody_count; i ++ )
	{
		// ボーンあわせ
		fix_position( i, frametime );
	}

	// シュミレーションを進める
	mmdpiBullet::advance_time_physical( fps, frametime );
	
	for( dword i = 0; i < rigidbody_count; i ++ )
	{
		// 物理演算によるボーン更新
		update_bone_physical( i );
	}
#endif
	return 0;
}

//==================
// ボーン位置あわせ
//==================
// ボーンに剛体の位置をあわせ
int mmdpiBone::fix_position( dword rigid_id, float fElapsedFrame )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( bullet_flag <= 0 )
		return -1;

	if( physics[ rigid_id ].rigidbody_type != 2 )
		return -1;
	
	// 剛体取得
	btRigidBody* body = getRigidBody( rigid_id );
	if( !body )
		return -1;

	dword bone_id = physics[ rigid_id ].bone_index;
	if( bone_num <= bone_id ) 
		return -1;
		
	mmdpiVector3d	vec3;
	vec3 = bone[ bone_id ].matrix.get_transform();

	btTransform trans = body->getCenterOfMassTransform();
	trans.setOrigin( btVector3( 0, 0, 0 ) );
	trans = physics_sys[ rigid_id ].offset_trans * trans;
	trans.setOrigin( trans.getOrigin() + btVector3( vec3.x, vec3.y, vec3.z ) );

	float fRate = 0.2f * fElapsedFrame;
	if( fRate > 1.0f )
		fRate = 1.0f;
	body->translate( ( trans.getOrigin() - body->getCenterOfMassTransform().getOrigin() ) * fRate );
#endif
	return 0;
}

//==================================================================================
// ボーンの姿勢を剛体の姿勢と一致させる(そのフレームのシミュレーション終了後に呼ぶ)
//==================================================================================
int mmdpiBone::update_bone_physical( dword rigid_id )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( bullet_flag <= 0 )
		return -1;

	if( physics[ rigid_id ].rigidbody_type == 0 ) 
		return -1;

	if( physics_sys[ rigid_id ].m_bNoCopyToBone )
		return -1;

	dword bone_id = physics[ rigid_id ].bone_index;

	btRigidBody* body = getRigidBody( rigid_id );
	if( !body )
		return -1;

	// 位置移動
	btTransform trans = body->getWorldTransform() * physics_sys[ rigid_id ].offset_trans_inv;
	trans.getOpenGLMatrix( &bone[ bone_id ].matrix[ 0 ] );
#endif
	return 0;
}

//========================================
// 剛体をボーンの位置へ強制的に移動させる
//========================================
int mmdpiBone::move_to_bone_pos( dword rigid_id )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( !bullet_flag )
		return -1;

	if( physics[ rigid_id ].rigidbody_type == 0 )
		return -1;

	dword bone_id = physics[ rigid_id ].bone_index;
	if( bone_num <= bone_id ) 
		return -1;
		
	// 剛体取得
	btRigidBody* body = getRigidBody( rigid_id );
	if( !body )
		return -1;

	// ボーンの座標を取得
	mmdpiVector3d	bone_vec = bone[ bone_id ].matrix.get_transform();
	
	// 剛体位置取得
	btTransform trans = body->getCenterOfMassTransform();
	trans.setOrigin( btVector3( 0.0f, 0.0f, 0.0f ) );
	trans = physics_sys[ rigid_id ].offset_trans * trans;

	trans.setOrigin( trans.getOrigin() + btVector3( bone_vec.x, bone_vec.y, bone_vec.z ) );
	trans.setBasis( body->getWorldTransform().getBasis() );

	// 設定
	body->setCenterOfMassTransform( trans );

	body->setLinearVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	body->setAngularVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	body->setInterpolationLinearVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	body->setInterpolationAngularVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	body->setInterpolationWorldTransform( body->getCenterOfMassTransform() );
	body->clearForces();
#endif
	return 0;
}

// 物理エンジン情報生成
int mmdpiBone::create_physical_info( void )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	// 重力設定
	if( rigidbody_count > 0 )
		setGravity( 9.8f * 8 );

	uint	center_index;
	for( center_index = 0; center_index < bone_num; center_index ++ )
	{
		//if( strncmp( pbone[ center_index ].bone_name, "センター", 20 ) == 0 )
		//	break;
		char*	bone_name = bone[ center_index ].name;

		if( ( unsigned char )bone_name[ 0 ] == 0x83
			&& ( unsigned char )bone_name[ 1 ] == 0x5A
			&& ( unsigned char )bone_name[ 2 ] == 0x83 
			&& ( unsigned char )bone_name[ 3 ] == 0x93
			&& ( unsigned char )bone_name[ 4 ] == 0x83 
			&& ( unsigned char )bone_name[ 5 ] == 0x5E
			&& ( unsigned char )bone_name[ 6 ] == 0x81
			&& ( unsigned char )bone_name[ 7 ] == 0x5B )
		{
			break;
		}
	}

	physics_sys = ( rigidbody_count )? new MMDPI_PHYSICAL_INFO[ rigidbody_count ] : 0x00 ;
	// 剛体情報
	for( uint i = 0; i < rigidbody_count; i ++ )
	{
		tagMMDPI_BULLET_TYPE rigid_type = MMDPI_BULLET_SHERE;

		switch( physics[ i ].type )
		{
		case 0: rigid_type = MMDPI_BULLET_SHERE;		break;		// 球
		case 1: rigid_type = MMDPI_BULLET_BOX;			break;		// 箱
		case 2: rigid_type = MMDPI_BULLET_CAPSULE;		break;		// カプセル
		}		

		dword j = physics[ i ].bone_index;

		float bone_pos[ 3 ];
		mmdpiMatrix*	bone_matrix_l;
		if( j < bone_num )  
			bone_matrix_l = &bone[ j ].matrix;
		else
			bone_matrix_l = &bone[ center_index ].matrix;
		mmdpiVector3d	vec3;
		float			delta_pos[ 3 ];
		vec3 = bone_matrix_l->get_transform();
		for( int k = 0; k < 3; k ++ )
		{
			bone_pos[ k ] = vec3[ k ];
			delta_pos[ k ] = physics[ i ].pos[ k ] - bone_pos[ k ];
		}

		// ボーンオフセット用トランスフォーム作成	
		btTransform bttrBoneOffset = matrix_to_btTrans( delta_pos, physics[ i ].rot );

		physics_sys[ i ].offset_trans = bttrBoneOffset;
		physics_sys[ i ].offset_trans_inv = physics_sys[ i ].offset_trans.inverse();
		
		float mass = physics[ i ].mass;
		//// キネマティック剛体は重さ、なし
		//if( physics[ i ].rigidbody_type == 0 )
		//	mass = 0;
	
		// 拡張情報
		physics_sys[ i ].rbInfo.fLinearDamping		= physics[ i ].ac_t;
		physics_sys[ i ].rbInfo.fAngularDamping		= physics[ i ].ac_r;
		physics_sys[ i ].rbInfo.fRestitution		= physics[ i ].repulsion;
		physics_sys[ i ].rbInfo.fFriction			= physics[ i ].friction;
		
		physics_sys[ i ].rbInfo.rigidbody_group_index = physics[ i ].group;
		//if( physics[ i ].not_touch_group_flag == 0xffff )
		//	physics[ i ].not_touch_group_flag = 0;
		physics_sys[ i ].rbInfo.rigidbody_group_mask = physics[ i ].not_touch_group_flag;

		physics_sys[ i ].rbInfo.offset = physics_sys[ i ].offset_trans;
		physics_sys[ i ].rbInfo.kinematicMatrix = &bone[ j ].matrix;

		physics_sys[ i ].rbInfo.kinematic_mode = 1;

		bone_pos[ 0 ] = physics[ i ].pos[ 0 ];
		bone_pos[ 1 ] = physics[ i ].pos[ 1 ];
		bone_pos[ 2 ] = physics[ i ].pos[ 2 ];

		//bone_pos[ 0 ] = bone_pos[ 0 ] + physics[ i ].pos[ 0 ];
		//bone_pos[ 1 ] = bone_pos[ 1 ] + physics[ i ].pos[ 1 ];
		//bone_pos[ 2 ] = bone_pos[ 2 ] + physics[ i ].pos[ 2 ];

		//if( rType == MMDPI_BULLET_SHERE )
		//{
		//	physics[ i ].size[ 0 ] *= 2;
		//	physics[ i ].size[ 1 ] *= 2;
		//	physics[ i ].size[ 2 ] *= 2;
		//}

		btTransform btt = matrix_to_btTrans( bone_pos, physics[ i ].rot );

		// 剛体セット
		create_rigidbody( 
			rigid_type, &btt, mass,
			( physics[ i ].rigidbody_type == 0 ), 
			physics[ i ].size[ 0 ], physics[ i ].size[ 1 ], physics[ i ].size[ 2 ],
			&physics_sys[ i ].rbInfo 
		);

		// 他の情報
		if( physics[ i ].bone_index == 0xFFFF )
			physics_sys[ i ].m_bNoCopyToBone = 1;
		else
			physics_sys[ i ].m_bNoCopyToBone = 0;
	}
	
	// ジョイント
	for( uint i = 0; i < joint_count; i ++ )
	{
		// 点と点のジョイント
		btTransform trans = matrix_to_btTrans( joint[ i ].pos, joint[ i ].rot );

		// 拡張情報
		MMDPI_BULLET_CONSTRAINT_INFO j_info;

		j_info.limit_pos1 = btVector3( 
			joint[ i ].trans_limit1[ 0 ],
			joint[ i ].trans_limit1[ 1 ],
			joint[ i ].trans_limit1[ 2 ] );

		j_info.limit_pos2 = btVector3( 
			joint[ i ].trans_limit2[ 0 ],
			joint[ i ].trans_limit2[ 1 ],
			joint[ i ].trans_limit2[ 2 ] );

		j_info.limit_rot1 = btVector3( 
			joint[ i ].rotate_limit1[ 0 ],
			joint[ i ].rotate_limit1[ 1 ],
			joint[ i ].rotate_limit1[ 2 ] );

		j_info.limit_rot2 = btVector3( 
			joint[ i ].rotate_limit2[ 0 ],
			joint[ i ].rotate_limit2[ 1 ],
			joint[ i ].rotate_limit2[ 2 ] );

		j_info.spring_pos = btVector3( 
			joint[ i ].spring_trans[ 0 ],
			joint[ i ].spring_trans[ 1 ],
			joint[ i ].spring_trans[ 2 ] );

		j_info.spring_rot = btVector3( 
			joint[ i ].spring_rotate[ 0 ] * ( float )M_PI / 180.0f,
			joint[ i ].spring_rotate[ 1 ] * ( float )M_PI / 180.0f,
			joint[ i ].spring_rotate[ 2 ] * ( float )M_PI / 180.0f );

		create_joint_p2p( joint[ i ].a_index, joint[ i ].b_index, &trans, &j_info );
	}
					
	// ボーンにあわせる
	for( dword i = 0; i < rigidbody_count; i ++ )
		move_to_bone_pos( i );
#endif
	return 0;
}

mmdpiBone::mmdpiBone()
{
	bone = 0x00;
	
	bullet_flag = 1;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	physics_sys = 0x00;
#endif
}

mmdpiBone::~mmdpiBone()
{
	delete[] bone;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	delete[] physics_sys;
#endif
}
