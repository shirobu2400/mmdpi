
#include "mmdpi.h"


int mmdpi::load( const char* model_name )
{
	int	result = 0;

	//	読み込み済み
	if( pmm )
		return -1;
	cc_create_tables();
	
	pmm = pmd = new mmdpiPmd();
	result = pmd->load( model_name );

	if( result )
	{
		delete pmd;
		pmd = 0x00;

		pmm = pmx = new mmdpiPmx();
		result = pmx->load( model_name );
		if( result )
		{
			delete pmx;
			return -1;	// error.
		}
		pmx->set_pmx();
	}

	// bone name to bone index
	pmm->set_bone_name2index();

	return result;
}

void mmdpi::draw( void )
{
	if( pmx )
		pmx->draw();
	if( pmd )
		pmd->draw();
}

void mmdpi::set_bone_matrix( uint bone_index, mmdpiMatrix& matrix )
{
	pmm->set_bone_matrix( bone_index, matrix );
}

void mmdpi::set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix )
{
	pmm->set_bone_matrix( bone_name, matrix );
}

int mmdpi::get_bone_num( void )
{
	return pmm->get_bone_num();
}

char* mmdpi::get_bone_name( int index, int coding_is_sjis )
{
	return pmm->get_bone_name( index, coding_is_sjis );
}
void mmdpi::set_projection_matrix( const GLfloat* p_projection_matrix )
{
	pmm->set_projection_matrix( p_projection_matrix );
}

void mmdpi::set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix )
{
	pmm->set_projection_matrix( p_projection_matrix );
}

mmdpiVmd* mmdpi::vmd( int index )
{
	return get_vmd( index );
}

mmdpiVmd* mmdpi::get_vmd( int index )
{
	if( index < 0 || vmd_list.size() <= ( unsigned )index )
		return 0x00;
	return vmd_list[ index ]; 
}

//	Vmd Loader
int mmdpi::vmd_load( const char* motion_name )
{
	mmdpiVmd*			lvmd = new mmdpiVmd();
	MMDPI_BONE_INFO_PTR		bone = 0x00;

	if( lvmd == 0x00 )
		return -1;

	if( pmx )
	{
		bone = pmx->get_bone();
		lvmd->create_bone_map_pmx( pmx->get_pmx_bone(), pmx->get_bone_num() );
		//lvmd->create_morph_map_pmx( mmdpiPmxLoad::morph, mmdpiPmxLoad::morph_num );
	}
	else if( pmd )
	{
		bone = pmd->get_bone();
		lvmd->create_bone_map( pmd->get_pmd_bone(), pmd->get_bone_num() );
		//lvmd->create_morph_map_pmx( mmdpiPmxLoad::morph, mmdpiPmxLoad::morph_num );
	}
	else
		return -1; // error.
	lvmd->load( motion_name );
	lvmd->set_bone( bone );

	vmd_list.push_back( lvmd );
	return 0;
}

int mmdpi::motion_load( const char* motion_name )
{
	return vmd_load( motion_name );
}

void mmdpi::set_fps( int fps )
{
	pmm->set_fps( fps );
}

mmdpi::mmdpi()
{
	pmx = 0x00;
	pmd = 0x00;
	pmm = 0x00;
}

mmdpi::~mmdpi()
{
	for( unsigned int i = 0; i < vmd_list.size(); i ++ )
		delete vmd_list[ i ];
	if( pmx )
		delete pmx;
	if( pmd )
		delete pmd;
}