
#include "pmx.h"

int mmdpiPmx::load( const char* pmx_name )
{
	//	読み込みと解析
	if( mmdpiPmxAnalyze::load( pmx_name ) )
		return -1;

	mmdpiPmxAnalyze::create_bone( mmdpiPmxLoad::bone, mmdpiPmxLoad::bone_num );
		
	//	bullet
	//	Make bone matrix
	this->global_matrix();
	phy_load_flag = 0;
	if( bullet_flag == 0 )
		return 0;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	mmdpiBone::bullet_flag = 0;
	if( mmdpiPmxLoad::p_rigid_num > 0 )
	{
		mmdpiBone::bullet_flag = 1;
		phy_load_flag = 1;

		mmdpiBone::rigidbody_count	= mmdpiPmxLoad::p_rigid_num;
		mmdpiBone::joint_count		= mmdpiPmxLoad::p_joint_num;
		mmdpiBone::physics		= mmdpiPmxLoad::p_rigid;
		mmdpiBone::joint		= mmdpiPmxLoad::p_joint;

		create_physical_info();
	}
#endif	

	//	成功
	return 0;
}

void mmdpiPmx::draw( void )
{
	mmdpiPmxDraw::draw();
}

void mmdpiPmx::set_bone_matrix( uint bone_index, mmdpiMatrix& matrix )
{
	mmdpiBone::set_bone_matrix( bone_index, matrix );
}

