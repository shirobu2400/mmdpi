#include "mmdpi.hpp"


int mmdpi::load( std::string model_name )
{
	int	result = 0;

	// 読み込み済み
	if( this->pmm )
		return -1;
	cc_create_tables();

	this->pmm = this->pmd = new mmdpiPmd();
	result = this->pmd->load( model_name.c_str() );

	if( result )
	{
		delete this->pmd;
		this->pmd = 0x00;

		this->pmm = pmx = new mmdpiPmx();
		result = pmx->load( model_name.c_str() );
		if( result )
		{
			delete this->pmx;
			return -1;	// error.
		}
		this->pmx->set_pmx();
	}

	// bone name to bone index
	this->pmm->set_bone_name2index();

	return result;
}

void mmdpi::draw( void )
{
	if( this->pmx )
		this->pmx->draw();
	if( this->pmd )
		this->pmd->draw();
}

void mmdpi::set_bone_matrix( uint bone_index, mmdpiMatrix& matrix )
{
	this->pmm->set_bone_matrix( bone_index, matrix );
}

void mmdpi::set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix )
{
	this->pmm->set_bone_matrix( bone_name, matrix );
}

int mmdpi::get_bone_num( void )
{
	return this->pmm->get_bone_num();
}

char* mmdpi::get_bone_name( int index, int coding_is_sjis )
{
	return this->pmm->get_bone_name( index, coding_is_sjis );
}
void mmdpi::set_projection_matrix( const GLfloat* p_projection_matrix )
{
	this->pmm->set_projection_matrix( p_projection_matrix );
}

void mmdpi::set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix )
{
	this->pmm->set_projection_matrix( p_projection_matrix );
}

mmdpiVmd* mmdpi::vmd( std::string name )
{
	return this->get_vmd( name );
}

mmdpiVmd* mmdpi::get_vmd( std::string name )
{
	if( this->vmd_instances.count( name ) == 0 )
		return 0x00;
	return this->vmd_instances[ name ];
}

//	Vmd Loader
int mmdpi::vmd_load( std::string motion_name, std::string file_name )
{
	mmdpiVmd*			lvmd = new mmdpiVmd();
	MMDPI_BONE_INFO_PTR		bone = 0x00;

	if( lvmd == 0x00 )
		return -1;

	if( this->pmx )
	{
		bone = this->pmx->get_bone();
		lvmd->create_bone_map_pmx( pmx->get_pmx_bone(), pmx->get_bone_num() );
		//lvmd->create_morph_map_pmx( mmdpiPmxLoad::morph, mmdpiPmxLoad::morph_num );
	}
	else if( pmd )
	{
		bone = this->pmd->get_bone();
		lvmd->create_bone_map( this->pmd->get_pmd_bone(), this->pmd->get_bone_num() );
		//lvmd->create_morph_map_pmx( mmdpiPmxLoad::morph, mmdpiPmxLoad::morph_num );
	}
	else
		return -1; // error.

	if( lvmd->load( file_name.c_str() ) )
		return -1;
	lvmd->set_bone( bone );

	this->vmd_instances[ motion_name ] = lvmd;
	return 0;
}

int mmdpi::motion_load( std::string motion_name, std::string file_name )
{
	return this->vmd_load( motion_name, file_name );
}

void mmdpi::set_fps( int fps )
{
	this->pmm->set_fps( fps );
}

mmdpi::mmdpi()
{
	this->pmx = 0x00;
	this->pmd = 0x00;
	this->pmm = 0x00;
}

mmdpi::~mmdpi()
{
	for( std::map<std::string, mmdpiVmd*>::iterator it = this->vmd_instances.begin(); it != this->vmd_instances.end(); it ++ )
		delete it->second;
	if( this->pmx )
		delete this->pmx;
	if( this->pmd )
		delete this->pmd;
}
