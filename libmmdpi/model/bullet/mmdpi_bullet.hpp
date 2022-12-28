
#include "../mmdpi_include.hpp"
#include "../mmdpi_struct.hpp"

#ifndef		__MMDPI__BULLET__DEFINES__
#define		__MMDPI__BULLET__DEFINES__	( 1 )

// SPU
//#define USE_PARALLEL_DISPATCHER

#include <time.h>

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h"
//
//#ifdef _DEBUG
//#define	_MMDPI_BULLET_PATH_	"bullet/lib/"
//
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "LinearMath_vs2010_debug.lib")
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "BulletCollision_vs2010_debug.lib")
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "BulletDynamics_vs2010_debug.lib")
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "BulletSoftBody_vs2010_debug.lib")
//
//#else
//#define	_MMDPI_BULLET_PATH_	"bullet/lib/"
//
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "LinearMath_vs2010.lib")
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "BulletCollision_vs2010.lib")
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "BulletDynamics_vs2010.lib")
//#pragma comment (lib, _MMDPI_BULLET_PATH_ "BulletSoftBody_vs2010.lib")
//
//#endif

#define MMDPI_SIGN( x ) ( ( x > 0 )? +1 : -1 )


// Quaternion
class MMDPI_QUATERNION : public btQuaternion {};


enum tagMMDPI_BULLET_TYPE{ MMDPI_BULLET_SHERE, MMDPI_BULLET_BOX, MMDPI_BULLET_CAPSULE, MMDPI_BULLET_CYLINDER, MMDPI_BULLET_CONE };

class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

///collisions between two btSoftBody's
class btSoftSoftCollisionAlgorithm;

///collisions between a btSoftBody and a btRigidBody
class btSoftRididCollisionAlgorithm;
class btSoftRigidDynamicsWorld;

typedef struct tagMMDPI_BULLET_RIGID_INFO
{
	btScalar		fLinearDamping;
	btScalar		fAngularDamping;
	btScalar		fRestitution;
	btScalar		fFriction;

	mmdpiMatrix*	kinematicMatrix;
	btTransform		offset;

	BYTE			rigidbody_group_index;
	ushort			rigidbody_group_mask;

	int kinematic_mode;
} MMDPI_BULLET_RIGID_INFO, *MMDPI_BULLET_RIGID_INFO_PTR;

typedef struct tagMMDPI_BULLET_CONSTRAINT_INFO
{
	btVector3		limit_pos1;
	btVector3		limit_pos2;

	btVector3		limit_rot1;
	btVector3		limit_rot2;

	btVector3		spring_pos;
	btVector3		spring_rot;
} MMDPI_BULLET_CONSTRAINT_INFO, *MMDPI_BULLET_CONSTRAINT_INFO_PTR;

typedef struct tagMMDPI_SHADOW_VERTEX
{
	mmdpiVector3d	pos;
	mmdpiVector3d	nor;
} MMDPI_SHADOW_VERTEX, *MMDPI_SHADOW_VERTEX_PTR;

typedef struct tagMMDPI_PHYSICAL_INFO
{
	btTransform					offset_trans;
	btTransform					offset_trans_inv;
	int						m_bNoCopyToBone;
	MMDPI_BULLET_RIGID_INFO				rbInfo;
} MMDPI_PHYSICAL_INFO, *MMDPI_PHYSICAL_INFO_PTR;


// Bullet
class mmdpiBullet
{
private:

	uint						rigidbody_num;
	uint						softbody_num;

	btSoftBodyWorldInfo				softWorldInfo;

	btAlignedObjectArray< btCollisionShape* >	collisionShapes;

	//btDefaultCollisionConfiguration*		collisionConfiguration;
	btCollisionConfiguration*			collisionConfiguration;

	btCollisionDispatcher*				dispatcher;
	btBroadphaseInterface*				overlappingPairCache;
	btSequentialImpulseConstraintSolver*		solver;

	// bulletWorld
	btDynamicsWorld*				dynamicsWorld;
	//btDiscreteDynamicsWorld*			dynamicsWorld;
	//btDynamicsWorld*				bt_world;
	//btTransform					groundTransform;

	int createShape( btCollisionShape **pColShape,
		tagMMDPI_BULLET_TYPE rigidbody_type,
		float width, float height, float depth );

	struct btKinematicMotionState : public btMotionState
	{
		btTransform	m_graphicsWorldTrans;
		btTransform	m_BoneOffset;
		btTransform	m_startWorldTrans;
		mmdpiMatrix	*m_pBoneMatrix;

		btKinematicMotionState( const btTransform& startTrans, const btTransform& boneOffset,
						mmdpiMatrix *pBoneMatrix )
						: m_BoneOffset( boneOffset ), m_startWorldTrans( startTrans ),
						m_pBoneMatrix( pBoneMatrix )
		{
		}

		///synchronizes world transform from user to physics
		virtual void getWorldTransform( btTransform& centerOfMassWorldTrans ) const
		{
			btTransform	bttrBoneTransform;

			bttrBoneTransform.setIdentity();
			if( m_pBoneMatrix )
				bttrBoneTransform.setFromOpenGLMatrix( ( const btScalar* )&m_pBoneMatrix[ 0 ] );

			centerOfMassWorldTrans = bttrBoneTransform * m_BoneOffset;
		}

		///synchronizes world transform from physics to user
		///Bullet only calls the update of worldtransform for active objects
		virtual void setWorldTransform( const btTransform& centerOfMassWorldTrans )
		{
			m_graphicsWorldTrans = centerOfMassWorldTrans;
		}

	};

	//	時間管理
	clock_t prev_time;

public:

	// 時間管理
	virtual int advance_time_physical( void );
	virtual int advance_time_physical( int fps, float frametime );

	//	フレーム操作
	virtual float getFrameTime( int fps )
	{
		clock_t	curtime = clock();
		float	frame_time;

		if( prev_time == 0 )
			prev_time = curtime;
		frame_time = ( float )( curtime - prev_time ) * ( ( float )fps / CLOCKS_PER_SEC );
		prev_time = curtime;

		return frame_time;
	}

	// 行列系
	mmdpiMatrix get_matrix( mmdpiMatrix *mOut, int object_id );
	int set_matrix( int object_id, mmdpiMatrix *mIn );

	// ソフト頂点
	int get_softbody_vertex( mmdpiVector3d *vertex, int id );

	// ソフト面
	int getFaceNum( int id );

	btSoftBody *getSoftBody( int id )
	{
		if( id < 0 || getSoftDynamicsWorld()->getSoftBodyArray().size() <= id )
			return 0x00;
		return getSoftDynamicsWorld()->getSoftBodyArray()[ id ];
	}
	btRigidBody *getRigidBody( int id )
	{
		if( id < 0 || getDiscreteDynamicsWorld()->getCollisionObjectArray().size() <= id )
			return 0x00;
		return ( btRigidBody* )getDiscreteDynamicsWorld()->getCollisionObjectArray()[ id ];
	}

	//	重力設定
	float setGravity( float gravity );

	// 剛体設定
	int create_rigidbody( tagMMDPI_BULLET_TYPE rigidbody_type,
		btTransform *trans,  float weight,
		int kinematic_flag,
		float width, float height, float depth,
		MMDPI_BULLET_RIGID_INFO_PTR rigid_info = 0 );

	// ソフトボディー
	int create_softbody( mmdpiVector3d *vertices, int *index, int index_num, btTransform &trans );

	// ジョイント操作
	int create_joint_p2p( int bodyId_a, int bodyId_b, btTransform* trans, MMDPI_BULLET_CONSTRAINT_INFO_PTR joint_info = 0x00 );

	// 変換系
	btTransform matrix_to_btTrans( float *pos, float *dir );
	btTransform matrix_to_btTransMatrix( mmdpiMatrix *matrix );
	mmdpiMatrix btTrans_to_matrix( btTransform *trans );

	// 衝突判定
	btAlignedObjectArray< btCollisionShape* > *getCollisionArray( void ){ return &collisionShapes; }
	btDynamicsWorld* getDynamicsWorld( void )
	{
		return dynamicsWorld;
	}
	btDiscreteDynamicsWorld* getDiscreteDynamicsWorld( void )
	{
		return ( btDiscreteDynamicsWorld* )dynamicsWorld;
	}
	btSoftRigidDynamicsWorld* getSoftDynamicsWorld( void )
	{
		///just make it a btSoftRigidDynamicsWorld please
		///or we will add type checking
		return ( btSoftRigidDynamicsWorld* )dynamicsWorld;
	}

	//btTransform* getGround( void ){ return &groundTransform; }

	mmdpiBullet();
	~mmdpiBullet();
} ;

#endif	//	__MMDPI__BULLET__DEFINES__
