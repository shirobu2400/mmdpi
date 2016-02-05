
#include "pmd.h"

int mmdpiPmd::load( const char* pmd_name )
{
	//	読み込みと解析
	if( mmdpiPmdAnalyze::load( pmd_name ) )
		return -1;

	mmdpiModel::create();
	
	mmdpiPmdAnalyze::create_bone( mmdpiPmdLoad::bone, mmdpiPmdLoad::bone_num );
		
	//	bullet
	//	Make bone matrix
	this->global_matrix();
	phy_load_flag = 0;
	if( bullet_flag == 0 )
		return 0;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	mmdpiBone::bullet_flag = 0;
	if( mmdpiPmdLoad::p_rigid_num > 0 )
	{
		mmdpiBone::bullet_flag = 1;
		phy_load_flag = 1;

		mmdpiBone::rigidbody_count		= mmdpiPmdLoad::p_rigid_num;
		mmdpiBone::joint_count			= mmdpiPmdLoad::p_joint_num;
		mmdpiBone::physics				= mmdpiPmdLoad::p_rigid;
		mmdpiBone::joint				= mmdpiPmdLoad::p_joint;

		create_physical_info();
	}
#endif	

	//	成功
	return 0;
}

void mmdpiPmd::draw( void )
{
	mmdpiPmdDraw::draw();
}

void mmdpiPmd::set_bone_matrix( uint bone_index, mmdpiMatrix& matrix )
{
	mmdpiBone::set_bone_matrix( bone_index, matrix );
}

