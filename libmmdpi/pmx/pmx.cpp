
#include "pmx.hpp"

int mmdpiPmx::load( const char* pmx_name )
{
	// 読み込みと解析
	if( this->mmdpiPmxAnalyze::load( pmx_name ) )
		return -1;

	this->mmdpiPmxAnalyze::create_bone( mmdpiPmxLoad::bone, mmdpiPmxLoad::bone_num );

	// bullet
	// Make bone matrix
	mmdpiBone::update_global_matrix_all();
	this->phy_load_flag = 0;
	if( this->bullet_flag == 0 )
		return 0;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	this->mmdpiBone::bullet_flag = 0;
	if( this->mmdpiPmxLoad::p_rigid_num > 0 )
	{
		this->mmdpiBone::bullet_flag = 1;
		this->phy_load_flag = 1;

		this->mmdpiBone::rigidbody_count	= this->mmdpiPmxLoad::p_rigid_num;
		this->mmdpiBone::joint_count		= this->mmdpiPmxLoad::p_joint_num;
		this->mmdpiBone::physics		= this->mmdpiPmxLoad::p_rigid;
		this->mmdpiBone::joint			= this->mmdpiPmxLoad::p_joint;

		create_physical_info();
	}
#endif

	// 成功
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

