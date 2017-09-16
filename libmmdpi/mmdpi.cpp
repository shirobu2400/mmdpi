
#include "mmdpi.h"


int mmdpi::load( const char* model_name )
{
	//	読み込み済み
	if( pmm )
		return -1;

	int		result = 0;
	pmm = pmd = new mmdpiPmd();
	if( pmd->load( model_name ) == 0 )
		return 0;
	delete pmd;
	pmd = 0x00;

	pmm = pmx = new mmdpiPmx();
	return pmx->load( model_name );
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
	//pmx? pmx->set_bone_matrix( bone_index, matrix ) : pmd? pmd->set_bone_matrix( bone_index, matrix ) : 0x00 ;
	//pmx->set_bone_matrix( bone_index, matrix );
}

void mmdpi::set_projection_matrix( const GLfloat* p_projection_matrix )
{
	pmm->set_projection_matrix( p_projection_matrix );
}

void mmdpi::set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix )
{
	pmm->set_projection_matrix( p_projection_matrix );
}

mmdpiVmd* mmdpi::get_vmd( int index )
{
	if( index < 0 || vmd.size() <= ( unsigned )index )
		return NULL;
	return vmd[ index ]; 
}

//	Vmd Loader
int mmdpi::vmd_load( const char *file_name )
{
	mmdpiVmd*			lvmd = new mmdpiVmd();
	MMDPI_BONE_INFO_PTR		bone = 0x00;

	if( pmx )
	{
		bone = pmx->get_bone();
		lvmd->create_bone_map_pmx( pmx->get_pmx_bone(), pmx->get_bone_num() );
		//lvmd->create_morph_map_pmx( mmdpiPmxLoad::morph, mmdpiPmxLoad::morph_num );
	}
	else	//if( pmd )
	{
		bone = pmd->get_bone();
		lvmd->create_bone_map( pmd->get_pmd_bone(), pmd->get_bone_num() );
		//lvmd->create_morph_map_pmx( mmdpiPmxLoad::morph, mmdpiPmxLoad::morph_num );
	}
	lvmd->load( file_name );
	lvmd->set_bone( bone );

	vmd.push_back( lvmd );

	return 0;
}

void mmdpi::set_fps( int fps )
{
	pmm->set_fps( fps );
	//pmx? pmx->set_fps( fps ) : pmd? pmd->set_fps( fps ) : 0x00 ;
}

mmdpi::mmdpi()
{
	pmx = 0x00;
	pmd = 0x00;
	pmm = 0x00;
}

mmdpi::~mmdpi()
{
	for( unsigned int i = 0; i < vmd.size(); i ++ )
		delete vmd[ i ];
	delete pmx;
	delete pmd;
}