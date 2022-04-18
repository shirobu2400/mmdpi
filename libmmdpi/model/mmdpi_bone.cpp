
#include "mmdpi_bone.hpp"


// matrix 分だけ変換する
int mmdpiBone::set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix )
{
	if( this->mmdpiBone::bone_num <= bone_index )
		return -1;
	this->bone[ bone_index ].delta_matrix = this->bone[ bone_index ].delta_matrix * matrix;
	return 0;
}

// ボーンローカルマテリックス生成
void mmdpiBone::update_local_matrix( void )
{
	for( uint i = 0; i < this->mmdpiBone::bone_num; i ++ )
		this->bone[ i ].local_matrix = this->bone[ i ].offset_matrix * this->bone[ i ].global_matrix;
}

// すべてのグローバル行列生成
int mmdpiBone::update_global_matrix_all( void )
{
	return this->update_global_matrix( &this->bone[ 0 ], 0x00 );
}

// グローバル行列生成
int mmdpiBone::update_global_matrix( MMDPI_BONE_INFO_PTR my_bone, const mmdpiMatrix* offset )
{
	if( my_bone == 0x00 )
		return -1;

	if( offset )
		my_bone->global_matrix = my_bone->temp_matrix * ( *offset ) * my_bone->delta_matrix;
	else
		my_bone->global_matrix = my_bone->temp_matrix * my_bone->delta_matrix;

	if( my_bone->first_child )
		this->update_global_matrix( my_bone->first_child, &my_bone->global_matrix );
	if( my_bone->sibling )
		this->update_global_matrix( my_bone->sibling, offset );

	return 0;
}

// PMX
mmdpiMatrix mmdpiBone::update_temp_matrix( int index )
{
	if( this->bone[ index ].parent == 0x00 )
		return this->bone[ index ].temp_matrix * this->bone[ index ].delta_matrix;
	return this->bone[ index ].temp_matrix * this->bone[ index ].parent->global_matrix * this->bone[ index ].delta_matrix;
}

// ボーンのモデル上のローカル座標系の取得
mmdpiMatrix mmdpiBone::get_global_matrix( MMDPI_BONE_INFO_PTR bone )
{
	if( bone->parent )
	{
		mmdpiMatrix	parent_matrix = get_global_matrix( bone->parent );
		return bone->temp_matrix * parent_matrix;
	}
	return bone->temp_matrix;
}

void mmdpiBone::refresh_bone_mat( void )
{
	// 初期化
	for( uint i = 0; i < this->bone_num; i ++ )
	{
		this->bone[ i ].temp_matrix = this->bone[ i ].init_matrix;
		this->bone[ i ].delta_matrix.initialize();
	}
}

// 初期位置設定
void mmdpiBone::compute_init_matrix( MMDPI_BONE_INFO_PTR my_bone, mmdpiMatrix* offset )
{
	if( my_bone->first_child )
		this->compute_init_matrix( my_bone->first_child, &my_bone->offset_matrix );
	if( my_bone->sibling )
		this->compute_init_matrix( my_bone->sibling, offset );
	if( offset )
		my_bone->init_matrix = ( *offset ) * my_bone->init_matrix;
}

mmdpiMatrix mmdpiBone::compute_init_matrix_bottom( MMDPI_BONE_INFO_PTR now_bone )
{
	if( now_bone->parent )
		return now_bone->parent->offset_matrix * now_bone->init_matrix;
	return now_bone->init_matrix;
}

// 物理演算再生
int mmdpiBone::advance_time_physical( int fps )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( bullet_flag <= 0 )
		return 0;

	float	frametime = this->mmdpiBullet::getFrameTime( fps );
	if( frametime > 10 )
		frametime = 10;

	for( dword i = 0; i < rigidbody_count; i ++ )
	{
		// ボーンあわせ
		this->fix_position( i, frametime );
	}

	// シュミレーションを進める
	this->mmdpiBullet::advance_time_physical( fps, frametime );

	for( dword i = 0; i < rigidbody_count; i ++ )
	{
		// 物理演算によるボーン更新
		this->update_bone_physical( i );
	}
#endif
	return 0;
}

// ボーンの位置に剛体の位置をあわせる
int mmdpiBone::fix_position( dword rigid_id, float fElapsedFrame )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( this->bullet_flag <= 0 )
		return -1;

	//// ボーン追従のみ終了
	//if( physics[ rigid_id ].rigidbody_type == 1 )
	//	return -1;

	//	剛体取得
	btRigidBody* 	body = this->mmdpiBullet::getRigidBody( rigid_id );
	if( body == 0x00 )
		return -1;

	dword 		bone_id = this->physics[ rigid_id ].bone_index;
	if( bone_num <= bone_id )
		return -1;

	mmdpiVector3d	vec3;
	vec3 = bone[ bone_id ].global_matrix.get_transform();

	btTransform trans = body->getCenterOfMassTransform();
	trans.setOrigin( btVector3( 0, 0, 0 ) );
	trans = physics_sys[ rigid_id ].offset_trans * trans;
	trans.setOrigin( trans.getOrigin() + btVector3( vec3.x, vec3.y, vec3.z ) );

	float fRate = 0.20f * fElapsedFrame;
	if( fRate > 1.00f )
		fRate = 1.00f;
	body->translate( ( trans.getOrigin() - body->getCenterOfMassTransform().getOrigin() ) * fRate );
#endif
	return 0;
}

// ボーンの姿勢を剛体の姿勢と一致させる(そのフレームのシミュレーション終了後に呼ぶ)
int mmdpiBone::update_bone_physical( dword rigid_id )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( this->bullet_flag <= 0 )
		return -1;

	// ボーンを剛体に合わせないフラグがたっているときは強制終了
	if( this->physics_sys[ rigid_id ].m_bNoCopyToBone )
		return -1;

	//// 物理演算のみ終了
	//if( physics[ rigid_id ].rigidbody_type == 1 )
	//	return -1;

	dword 		bone_id = this->physics[ rigid_id ].bone_index;
	btRigidBody* 	body = this->mmdpiBullet::getRigidBody( rigid_id );
	if( body == 0x00 )
		return -1;

	// 位置移動
	btTransform trans = body->getWorldTransform() * physics_sys[ rigid_id ].offset_trans_inv;
	trans.getOpenGLMatrix( &bone[ bone_id ].global_matrix[ 0 ] );
#endif
	return 0;
}

// 剛体をボーンの位置へ強制的に移動させる
int mmdpiBone::move_to_bone_pos( dword rigid_id )
{
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	if( this->bullet_flag == 0 )
		return -1;

	// ボーンを剛体に合わせないフラグがたっているときは強制終了
	if( this->physics_sys[ rigid_id ].m_bNoCopyToBone )
		return -1;

	//// ボーン追従のみ終了
	//if( this->physics[ rigid_id ].rigidbody_type == 0 )
	//	return -1;

	dword		bone_id = this->physics[ rigid_id ].bone_index;
	if( bone_num <= bone_id )
		return -1;

	// 剛体取得
	btRigidBody* 	body = this->mmdpiBullet::getRigidBody( rigid_id );
	if( !body )
		return -1;

	// ボーンの座標を取得
	mmdpiVector3d	bone_vec = bone[ bone_id ].global_matrix.get_transform();

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

	// 剛体が存在しない
	if( this->rigidbody_count == 0 )
		return 0;

	// 重力設定
	this->::mmdpiBullet::setGravity( 9.8f );

	uint	center_index;
	for( center_index = 0; center_index < this->bone_num; center_index ++ )
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

	this->physics_sys = new MMDPI_PHYSICAL_INFO[ rigidbody_count ];
	if( this->physics_sys == 0x00 )
		return -1;

	// 剛体情報
	for( uint i = 0; i < this->rigidbody_count; i ++ )
	{
		tagMMDPI_BULLET_TYPE	rigid_type = MMDPI_BULLET_SHERE;

		switch( this->physics[ i ].type )
		{
		case 0: rigid_type = MMDPI_BULLET_SHERE;	break;		// 球
		case 1: rigid_type = MMDPI_BULLET_BOX;		break;		// 箱
		case 2: rigid_type = MMDPI_BULLET_CAPSULE;	break;		// カプセル
		}

		dword		j = this->physics[ i ].bone_index;

		float		bone_pos[ 3 ];
		mmdpiMatrix*	bone_matrix_l;
		if( j < bone_num )
			bone_matrix_l = &this->bone[ j ].global_matrix;
		else
		{
			bone_matrix_l = &this->bone[ center_index ].global_matrix;
			j = center_index;
		}

		mmdpiVector3d		vec3;
		float			delta_pos[ 3 ];

		vec3 = bone_matrix_l->get_transform();
		for( int k = 0; k < 3; k ++ )
		{
			bone_pos[ k ] = vec3[ k ];
			delta_pos[ k ] = this->physics[ i ].pos[ k ] - bone_pos[ k ];
		}

		// ボーンオフセット用トランスフォーム作成
		btTransform 	bttrBoneOffset = this->matrix_to_btTrans( delta_pos, physics[ i ].rot );

		this->physics_sys[ i ].offset_trans = bttrBoneOffset;
		this->physics_sys[ i ].offset_trans_inv = this->physics_sys[ i ].offset_trans.inverse();

		float	mass = this->physics[ i ].mass;

		// キネマティック剛体は重さ、なし
		if( this->physics[ i ].rigidbody_type == 0 )
			mass = 0;

		// 拡張情報
		this->physics_sys[ i ].rbInfo.fLinearDamping	= this->physics[ i ].ac_t;
		this->physics_sys[ i ].rbInfo.fAngularDamping	= this->physics[ i ].ac_r;
		this->physics_sys[ i ].rbInfo.fRestitution	= this->physics[ i ].repulsion;
		this->physics_sys[ i ].rbInfo.fFriction		= this->physics[ i ].friction;

		this->physics_sys[ i ].rbInfo.rigidbody_group_index = this->physics[ i ].group;
		//if( physics[ i ].not_touch_group_flag == 0xffff )
		//	physics[ i ].not_touch_group_flag = 0;
		this->physics_sys[ i ].rbInfo.rigidbody_group_mask = this->physics[ i ].not_touch_group_flag;

		this->physics_sys[ i ].rbInfo.offset = this->physics_sys[ i ].offset_trans;
		this->physics_sys[ i ].rbInfo.kinematicMatrix = &this->bone[ j ].global_matrix;

		this->physics_sys[ i ].rbInfo.kinematic_mode = 1;

		bone_pos[ 0 ] = this->physics[ i ].pos[ 0 ];
		bone_pos[ 1 ] = this->physics[ i ].pos[ 1 ];
		bone_pos[ 2 ] = this->physics[ i ].pos[ 2 ];

		//bone_pos[ 0 ] = bone_pos[ 0 ] + physics[ i ].pos[ 0 ];
		//bone_pos[ 1 ] = bone_pos[ 1 ] + physics[ i ].pos[ 1 ];
		//bone_pos[ 2 ] = bone_pos[ 2 ] + physics[ i ].pos[ 2 ];

		//if( rType == MMDPI_BULLET_SHERE )
		//{
		//	physics[ i ].size[ 0 ] *= 2;
		//	physics[ i ].size[ 1 ] *= 2;
		//	physics[ i ].size[ 2 ] *= 2;
		//}

		btTransform 	btt = this->mmdpiBullet::matrix_to_btTrans( bone_pos, this->physics[ i ].rot );

		// 他の情報
		this->physics_sys[ i ].m_bNoCopyToBone = 0;	// 基本的にボーンを剛体へ合わせる
		if( this->physics[ i ].bone_index == 0xffff )
			this->physics_sys[ i ].m_bNoCopyToBone = 1;	// Bone を合わせないフラグ

		// 剛体セット
		this->mmdpiBullet::create_rigidbody(
			rigid_type, &btt, mass,
			( this->physics[ i ].rigidbody_type == 0 ),
			this->physics[ i ].size[ 0 ], this->physics[ i ].size[ 1 ], this->physics[ i ].size[ 2 ],
			&this->physics_sys[ i ].rbInfo
		);
	}

	//	ジョイント
	for( uint i = 0; i < this->joint_count; i ++ )
	{
		// 点と点のジョイント
		btTransform			trans = this->mmdpiBullet::matrix_to_btTrans( joint[ i ].pos, joint[ i ].rot );

		// 拡張情報
		MMDPI_BULLET_CONSTRAINT_INFO	j_info;

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
			joint[ i ].spring_rotate[ 0 ],
			joint[ i ].spring_rotate[ 1 ],
			joint[ i ].spring_rotate[ 2 ] );

		this->mmdpiBullet::create_joint_p2p( joint[ i ].a_index, joint[ i ].b_index, &trans, &j_info );
	}

	// ボーンにあわせる
	for( dword i = 0; i < this->rigidbody_count; i ++ )
		this->move_to_bone_pos( i );
#endif
	return 0;
}

mmdpiBone::mmdpiBone()
{
	this->bone = 0x00;
	this->bone_num = 0;

	this->bullet_flag = 1;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	this->physics_sys = 0x00;
#endif
}

mmdpiBone::~mmdpiBone()
{
	delete[] this->bone;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	delete[] this->physics_sys;
#endif
}
