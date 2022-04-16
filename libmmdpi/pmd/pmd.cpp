
#include "pmd.hpp"

int mmdpiPmd::load( const char* pmd_name )
{
	// 読み込みと解析
	if( this->mmdpiPmdAnalyze::load( pmd_name ) )
		return -1;

	this->mmdpiPmdAnalyze::create_bone( this->mmdpiPmdLoad::bone, this->mmdpiPmdLoad::bone_num );

	// bullet
	// Make bone matrix
	this->global_matrix();
	this->phy_load_flag = 0;
	if( this->bullet_flag == 0 )
		return 0;
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	this->mmdpiBone::bullet_flag = 0;
	if( this->mmdpiPmdLoad::p_rigid_num > 0 )
	{
		this->mmdpiBone::bullet_flag = 1;
		this->phy_load_flag = 1;

		this->mmdpiBone::rigidbody_count	= mmdpiPmdLoad::p_rigid_num;
		this->mmdpiBone::joint_count		= mmdpiPmdLoad::p_joint_num;
		this->mmdpiBone::physics		= mmdpiPmdLoad::p_rigid;
		this->mmdpiBone::joint			= mmdpiPmdLoad::p_joint;

		this->create_physical_info();
	}
#endif

	// 成功
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

