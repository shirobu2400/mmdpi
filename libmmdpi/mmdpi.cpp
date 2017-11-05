
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
	//pmx? pmx->set_bone_matrix( bone_index, matrix ) : pmd? pmd->set_bone_matrix( bone_index, matrix ) : 0x00 ;
	//pmx->set_bone_matrix( bone_index, matrix );
}

void mmdpi::set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix )
{
	pmm->set_bone_matrix( bone_name, matrix );
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
	if( pmx )
		delete pmx;
	if( pmd )
		delete pmd;
}