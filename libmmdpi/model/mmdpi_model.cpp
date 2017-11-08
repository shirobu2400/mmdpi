
#include "mmdpi_model.h"
#include "../model/tools/get_bin.h"


int mmdpiModel::create( void )
{
	if( mmdpiShader::default_shader() )
		return -1;	//	Error
	//	Success
	return 0;
}

void mmdpiModel::draw( void )
{
	////	Morph
	//skin_init_update();
	//for( uint i = 0; i < morph_num; i ++ )
	//{
	//	if( morph[ i ].morph_flag )
	//		morph_exec( i, +1.0f );
	//}
	//skin_update( 0, 1 );
	//skin_update( 0 );

	//	ローカル行列に変換
	this->make_local_matrix();
	this->refresh_bone_mat();

	//	ボーン行列をシェーダに送る準備
	for( dword i = 0; i < mesh.size(); i ++ )
		mesh[ i ]->update_matrix( mmdpiBone::bone, mmdpiBone::bone_num );

	//	ProjectioMatrix
	mmdpiShader::set_projection_matrix( &projection_matrix );

	//	Using My Shader
	shader_on();

	//	表示
	for( dword i = 0; i < mesh.size(); i ++ )
		mesh[ i ]->draw();

	//	Not Using My Shader
	shader_off();
}

void mmdpiModel::set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix )
{
	mmdpiBone::set_bone_matrix( bone_index, matrix );
}

void mmdpiModel::set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix )
{
	string					name( bone_name );
	map<const string, uint>::iterator	itr;

	itr = this->bone_name2index_sjis.find( name );
	if( itr == this->bone_name2index_sjis.end() )	// not found
		itr = this->bone_name2index_utf8.find( name );
	if( itr == this->bone_name2index_utf8.end() )	// not found
		return ;
	this->set_bone_matrix( itr->second, matrix );
}

int mmdpiModel::get_bone_num( void )
{
	return ( signed int )bone_num;
}

char* mmdpiModel::get_bone_name( int index, int coding_is_sjis )
{
	if( index < 0 || ( signed )bone_num <= index )
		return 0x00;

	MMDPI_BONE_INFO_PTR	this_bone = &mmdpiBone::bone[ index ];
	if( coding_is_sjis && !is_pmd )
		return this_bone->sjis_name;
	return this_bone->name;
}

void mmdpiModel::set_projection_matrix( const GLfloat* p_projection_matrix )
{
	for( int i = 0; i < 16; i ++ )
		projection_matrix[ i ] = p_projection_matrix[ i ];
}

void mmdpiModel::set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix )
{
	for( int i = 0; i < 16; i ++ )
		projection_matrix[ i ] = ( *p_projection_matrix )[ i ];
}

void mmdpiModel::set_pmx( void )
{
	this->is_pmd = 0;
}

int mmdpiModel::set_bone_name2index( void )
{
	if( is_pmd )
	{
		for( uint i = 0; i < this->bone_num; i ++ )
		{
			char	tempc[ 32 ] = { 0 };

			this->bone_name2index_sjis[ this->bone[ i ].name ] = i;
			cconv_sjis_to_utf8( tempc, this->bone[ i ].name );			
			this->bone_name2index_utf8[ tempc ] = i;
		}

		return 0;
	}

	// pmx
	for( uint i = 0; i < this->bone_num; i ++ )
	{
		this->bone_name2index_utf8[ this->bone[ i ].name      ] = i;
		this->bone_name2index_sjis[ this->bone[ i ].sjis_name ] = i;
	}
	return 0;
}

int mmdpiModel::set_physics_engine( int type )
{
	return bullet_flag = type;
}

int mmdpiModel::set_fps( int fps )
{
	return this->fps = fps;
}
int mmdpiModel::get_fps( void )
{
	return fps;
}

mmdpiModel::mmdpiModel()
{
	bone_name = 0x00;
	fps = 30;
	is_pmd = 1;
}

mmdpiModel::~mmdpiModel()
{
	if( bone_name )
		delete[] bone_name;
}