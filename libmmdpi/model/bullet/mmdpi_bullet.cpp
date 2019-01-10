
#include "mmdpi_bullet.h"


int mmdpiBullet::advance_time_physical( int fps, float frametime )
{
	// 1ループにかかった時間を計測  
	float	wait_time_local = 1.0f / fps;
	float	fMilliSec = frametime * 1000.0f * wait_time_local;
	getDynamicsWorld()->stepSimulation( fMilliSec, _MMDPI_BULLET_STEP_/*, wait_time_local / 1000.0f*/ ); 
	return 0;  
}

int mmdpiBullet::advance_time_physical( void )
{
	//1ループにかかった時間を計測  
	//static dword prev_time = 0;
	//dword curtime = GetTickCount();
	//float frame_time = ( float )( curtime - prev_time ) / 1000.0f;  
 //   prev_time = curtime;  
	//getDynamicsWorld()->stepSimulation( frame_time ); 
	return 0;  
}

int mmdpiBullet::set_matrix( int object_id, mmdpiMatrix *mIn )
{
	//btCollisionObject* obj = getDiscreteDynamicsWorld()->getCollisionObjectArray()[ rigidbody_id ];
	btCollisionObject* obj = getDynamicsWorld()->getCollisionObjectArray()[ object_id ];
	btTransform trans;  // bulletから情報を取得
	btRigidBody* body = btRigidBody::upcast( obj );
	//mmdpiMatrix m = *mIn; //m._43 = m._43;
	body->getMotionState()->setWorldTransform( matrix_to_btTransMatrix( mIn ) );
	return 0;
}

mmdpiMatrix mmdpiBullet::get_matrix( mmdpiMatrix *mOut, int object_id )
{
	mmdpiMatrix		matrix, matrix_t;

	//btCollisionObject*	obj = getDiscreteDynamicsWorld()->getCollisionObjectArray()[ rigidbody_id ];
	btCollisionObject*	obj = getDynamicsWorld()->getCollisionObjectArray()[ object_id ];
	btTransform		trans;  // bulletから情報を取得
	btRigidBody*		body = btRigidBody::upcast( obj );
	body->getMotionState()->getWorldTransform( trans );

	// 行列取得
	//float m[ 16 ];
	//trans.getOpenGLMatrix( m );  
	//dxo_ConvertMatrixOpenGLToDirectx( ( float * )&matrix, m );
	//trans.getOpenGLMatrix( ( btScalar * )&matrix );

	//btMatrix3x3 rot = trans.getBasis();  
	//btVector3 euler;  
	//rot.getEulerZYX( euler[ 2 ], euler[ 1 ], euler[ 0 ] );  

	//dxo_MatrixTransRotate( &matrix, 
	//	trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ(),
	//	euler[ 0 ], euler[ 1 ], euler[ 2 ] );  

	trans.getOpenGLMatrix( ( btScalar * )&matrix[ 0 ] );

	if( mOut )
		*mOut = matrix;

	return matrix;
}

int mmdpiBullet::create_joint_p2p( int bodyId_a, int bodyId_b, btTransform* trans, MMDPI_BULLET_CONSTRAINT_INFO_PTR joint_info )
{
	//btVector3	jb_a( joint_a->x, joint_a->y, joint_a->z );
	//btVector3	jb_b( joint_b->x, joint_b->y, joint_b->z );
	btRigidBody*	rigid_a = getRigidBody( bodyId_a );
	btRigidBody*	rigid_b = getRigidBody( bodyId_b );

	if( rigid_a == 0x00 || rigid_b == 0x00 )
		return -1;

	//btTypedConstraint
	btGeneric6DofSpringConstraint* 
		p2p = new btGeneric6DofSpringConstraint
		( 
			*rigid_a, *rigid_b, 
			rigid_a->getWorldTransform().inverse() * ( *trans ), 
			rigid_b->getWorldTransform().inverse() * ( *trans ),
			true
		);
	//btTypedConstraint* p2p 
	//	= new btPoint2PointConstraint( 
	//	*getRigidBody( bodyId_a ),
	//	*getRigidBody( bodyId_b ),
	//	jb_a, jb_b );

	// 追加情報
	if( joint_info )
	{
		// 各種制限パラメータのセット
		p2p->setLinearLowerLimit( joint_info->limit_pos1 );
		p2p->setLinearUpperLimit( joint_info->limit_pos2 );

		p2p->setAngularLowerLimit( joint_info->limit_rot1 );
		p2p->setAngularUpperLimit( joint_info->limit_rot2 );

		// 0 : translation X
		if( fabs( joint_info->spring_pos.getX() ) > 1e-4 )
		{
			p2p->enableSpring( 0, true );
			p2p->setStiffness( 0, joint_info->spring_pos.getX() );
		}

		// 1 : translation Y
		if( fabs( joint_info->spring_pos.getY() ) > 1e-4 )
		{
			p2p->enableSpring( 1, true );
			p2p->setStiffness( 1, joint_info->spring_pos.getY() );
		}
		
		// 2 : translation Z
		if( fabs( joint_info->spring_pos.getZ() ) > 1e-4 )
		{
			p2p->enableSpring( 2, true );
			p2p->setStiffness( 2, joint_info->spring_pos.getZ() );
		}

		// 3 : rotation X (3rd Euler rotational around new position of X axis, range [-PI+epsilon, PI-epsilon] )
		// 4 : rotation Y (2nd Euler rotational around new position of Y axis, range [-PI/2+epsilon, PI/2-epsilon] )
		// 5 : rotation Z (1st Euler rotational around Z axis, range [-PI+epsilon, PI-epsilon] )
		p2p->enableSpring( 3, true );	p2p->setStiffness( 3, joint_info->spring_rot.getX() );
		p2p->enableSpring( 4, true );	p2p->setStiffness( 4, joint_info->spring_rot.getY() );
		p2p->enableSpring( 5, true );	p2p->setStiffness( 5, joint_info->spring_rot.getZ() );
	}

	//　ワールドに追加
	getDynamicsWorld()->addConstraint( p2p, true );

	return 0;
}

int mmdpiBullet::getFaceNum( int id )
{
	btSoftBody*	psb = getSoftDynamicsWorld()->getSoftBodyArray()[ id ];
	return psb->m_faces.size();
}

int mmdpiBullet::get_softbody_vertex( mmdpiVector3d *vertex, int id )
{
	btSoftBody*	psb = getSoftDynamicsWorld()->getSoftBodyArray()[ id ];
	int		face_num = psb->m_faces.size();
	int		vertex_num = face_num * 3;

	if( vertex )
	{
		for( int i = 0; i < vertex_num; i ++ )
		{
			const btVector3& position = psb->m_faces[ i / 3 ].m_n[ i % 3 ]->m_x;
			vertex[ i ].x = position.x();
			vertex[ i ].y = position.y();
			vertex[ i ].z = position.z();
		}
	}

	return face_num;
}

int mmdpiBullet::create_softbody( mmdpiVector3d *vertices, int *index, int index_num, btTransform &trans )
{
	/*
	int vertex_stride	= 3 * sizeof( btScalar );
	int index_stride	= 3 * sizeof( int );

	// 生成
	btSoftBody* softbody = btSoftBodyHelpers::CreateFromTriMesh( softWorldInfo, ( btScalar * )&vertices[ 0 ], index, index_num / 3 );
	softbody->getCollisionShape()->setMargin( 0.01f );

	////初期位置の設定 
	softbody->transform( trans );

	softbody->setTotalMass( 1.0f );				// 全体の質量
	softbody->m_cfg.collisions |= btSoftBody::fCollision::VF_SS;
	softbody->m_cfg.kVC = 20.0f;
	softbody->setPose( true, false );
	softbody->generateBendingConstraints( 2 );

	////登録
	getSoftDynamicsWorld()->addSoftBody( softbody );
	getCollisionArray()->push_back( ( btCollisionShape * )softbody );
	*/
	return softbody_num ++;
}

int mmdpiBullet::createShape( btCollisionShape **pColShape, 
	tagMMDPI_BULLET_TYPE rigidbody_type, 
	float width, float height, float depth )
{
	const float _scale_ = 0.50f;	// 立方体のスケーリング

	// シェープ作成
	switch( rigidbody_type )
	{
		case MMDPI_BULLET_SHERE:	// 球
		{
			btScalar s( width );
			*pColShape = new btSphereShape( s );	
		} break;
		case MMDPI_BULLET_BOX:	// 箱
		{
			btVector3 v( width, height, depth );
			v = v * _scale_;
			*pColShape = new btBoxShape( v );
		} break;
		case MMDPI_BULLET_CAPSULE:	// カプセル
		{
			btScalar s( width );
			btScalar s2( height );
			*pColShape = new btCapsuleShape( s, s2 );
		} break;
		case MMDPI_BULLET_CYLINDER:	// 円錐
		{
			return -1;	// 未実装
		} break;
		case MMDPI_BULLET_CONE:	// 円錐
		{
			return -1;	// 未実装
		} break;
	}

	return 0;
}

int mmdpiBullet::create_rigidbody( tagMMDPI_BULLET_TYPE rigidbody_type,
	btTransform *trans,  float weight,
	int kinematic_flag, 
	float width, float height, float depth,  
	MMDPI_BULLET_RIGID_INFO_PTR rigid_info )
{
	btCollisionShape*	pColShape;

	btScalar		mass( weight );  // 質量0なら静的ボディ
	btTransform		rigid_trans = *trans;

	//btDefaultMotionState* myMotionState = 0x00;
	btMotionState*		myMotionState = 0x00;
	if( kinematic_flag && rigid_info->kinematic_mode )
		myMotionState = new btKinematicMotionState( rigid_trans, rigid_info->offset, rigid_info->kinematicMatrix );
	else
		myMotionState = new btDefaultMotionState( rigid_trans );  

	// シェープ作成
	createShape( &pColShape, rigidbody_type, width, height, depth );

	if( kinematic_flag )
		mass = 0;

	bool		is_dynamic = ( mass != 0 );

	btVector3	localInertia( 0, 0, 0 );
	if( is_dynamic )
		pColShape->calculateLocalInertia( mass, localInertia );
		
	btRigidBody::btRigidBodyConstructionInfo	rbInfo( mass, myMotionState, pColShape, localInertia );  

	if( rigid_info )
	{
		rbInfo.m_linearDamping		= rigid_info->fLinearDamping;		// 移動減
		rbInfo.m_angularDamping		= rigid_info->fAngularDamping;		// 回転減
		rbInfo.m_restitution		= rigid_info->fRestitution;		// 反発力
		rbInfo.m_friction		= rigid_info->fFriction;		// 摩擦力
		rbInfo.m_additionalDamping	= true;
	}

	// 剛体生成
	btRigidBody*	body = new btRigidBody( rbInfo );  

	// キネマティクス剛体
	if( kinematic_flag )
	{
		body->setCollisionFlags( body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
		body->setActivationState( DISABLE_DEACTIVATION ) ;
	}
	body->setSleepingThresholds( 0.0f, 0.0f );

	getCollisionArray()->push_back( pColShape );			// 衝突判定に追加
	if( rigid_info )	// 拡張情報つき
	{
		ushort	g_index = 0x0001 << rigid_info->rigidbody_group_index;
		ushort	g_mask = rigid_info->rigidbody_group_mask;
		//if( g_mask == 0xffff )
		//	g_mask = 0;
		getDiscreteDynamicsWorld()->addRigidBody( body,	g_index, g_mask );		// ワールドに追加
	}
	else
		getDiscreteDynamicsWorld()->addRigidBody( body );		// ワールドに追加

	return rigidbody_num ++;	// 正常終了 (戻り値 => 剛体ID)
}

// 姿勢、移動
btTransform mmdpiBullet::matrix_to_btTrans( float* position, float* direction )
{
	btTransform	transform;

	// 初期化
	transform.setIdentity();

	// 角度
	if( direction )
	{
		btMatrix3x3		bt_matrix;
		bt_matrix.setIdentity();
		bt_matrix.setEulerZYX( direction[ 0 ], direction[ 1 ], direction[ 2 ] );
		transform.setBasis( bt_matrix );
	}

	// 位置
	if( position )
	{
		transform.setOrigin( btVector3( position[ 0 ], position[ 1 ], position[ 2 ] ) );
	}

	return transform;
}

// 姿勢、移動 => 行列
btTransform mmdpiBullet::matrix_to_btTransMatrix( mmdpiMatrix *matrix )
{
	btTransform trans;

	// 初期化
	trans.setIdentity();

	// 処理
	trans.setFromOpenGLMatrix( ( float * )&matrix[ 0 ] );

	return trans;
}

mmdpiMatrix mmdpiBullet::btTrans_to_matrix( btTransform *trans )
{
	mmdpiMatrix matrix;
	
	trans->getOpenGLMatrix( ( float * )&matrix[ 0 ] );

	return matrix;
}

mmdpiBullet::mmdpiBullet()
{
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	//collisionConfiguration = new btDefaultCollisionConfiguration();

	///register some softbody collision algorithms on top of the default btDefaultCollisionConfiguration
	collisionConfiguration = //new btSoftBodyRigidBodyCollisionConfiguration();
		new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher( collisionConfiguration );

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	//overlappingPairCache = new btDbvtBroadphase();
	// 物理演算の有効領域の指定
	overlappingPairCache = new btAxisSweep3( 
				btVector3( -_MMDPI_BULLET_SPACE_, -_MMDPI_BULLET_SPACE_, -_MMDPI_BULLET_SPACE_ ),
				btVector3(  _MMDPI_BULLET_SPACE_,  _MMDPI_BULLET_SPACE_,  _MMDPI_BULLET_SPACE_ ) 
			);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver();

	//dynamicsWorld = ( btDynamicsWorld* )( new btSoftRigidDynamicsWorld( dispatcher, overlappingPairCache, solver, collisionConfiguration ) );
	dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, overlappingPairCache, solver, collisionConfiguration );

	///-----initialization_end-----

	// ソフトボディーワールド情報
	softWorldInfo.m_broadphase = overlappingPairCache;
	softWorldInfo.m_dispatcher = dispatcher;
	softWorldInfo.m_sparsesdf.Initialize();
	softWorldInfo.air_density = 1.2f;
	softWorldInfo.water_density = 0.0f;
	softWorldInfo.water_offset = 0.0f; 
	softWorldInfo.water_normal.setValue( 0.0f, 0.0f, 0.0f );

	softbody_num = rigidbody_num = 0;

	prev_time = 0;
}

float mmdpiBullet::setGravity( float gravity )
{	
	getDynamicsWorld()->setGravity( btVector3( 0, -gravity, 0 ) );
	return gravity;
}

mmdpiBullet::~mmdpiBullet()
{
	// ボディー
	for( int i = 0; i < getDiscreteDynamicsWorld()->getNumCollisionObjects() - 1; i ++ )  
	{
		btCollisionObject* obj = getDiscreteDynamicsWorld()->getCollisionObjectArray()[ i ];
		btRigidBody* body = btRigidBody::upcast( obj );
		getDiscreteDynamicsWorld()->removeCollisionObject( obj );
		delete obj;
	}
	for( int j = 0; j < collisionShapes.size(); j ++ )  
	{  
		btCollisionShape* shape = collisionShapes[ j ];
		delete shape;  
	}  	
	
	// ジョイント
	for( int i = 0; i < getDynamicsWorld()->getNumConstraints(); i ++ )
	{
		btTypedConstraint* p2p = getDynamicsWorld()->getConstraint( i );
		//getDynamicsWorld()->removeConstraint( p2p );
		delete p2p;
	}

	collisionShapes.clear();  

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	///-----cleanup_end-----
}
