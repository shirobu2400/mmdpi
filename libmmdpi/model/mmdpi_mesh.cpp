#include "mmdpi_mesh.hpp"


int mmdpiMesh::draw( void )
{
	//// Morph
	//skin_init_update();
	//for( uint i = 0; i < morph_num; i ++ )
	//{
	//	if( morph[ i ].morph_flag )
	//		morph_exec( i, +1.0f );
	//}
	//skin_update( 0, 1 );
	//skin_update( 0 );

	// GLSL
	// Input shader infomation

	glEnable( GL_DEPTH_TEST );
	if( b_piece->is_draw_both )
	{
		glDisable( GL_CULL_FACE );
		draw_main( 1 );
	}
	else
	{
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );

		//glCullFace( GL_FRONT );
		//draw_main( 0 );

		glCullFace( GL_BACK );
		draw_main( 1 );
	}

	return 0;
}

int mmdpiMesh::draw_main( int cull_flag )
{
	MMDPI_PIECE*		piece = this->b_piece;
	MMDPI_MATERIAL_PTR	material = piece->raw_material;

	glActiveTexture( GL_TEXTURE0 );

	this->shader->init_material();
	if( material->texture.get_id() > 0 )
		glBindTexture( GL_TEXTURE_2D, material->texture.get_id() );
	//if( m->texture.get_id() > 0 )
	//	glUniform1f( m->texture.get_id(), 0 );

	this->shader->send_bone( piece );

	this->shader->set_alpha_for_shader( piece->opacity );

	if( cull_flag == 0 )
	{
		this->shader->set_edge_size( piece->edge_size );
		this->shader->set_edge_color( ( float* )piece->edge_color.ptr() );
	}
	else
		this->shader->set_edge_size( 0 );

	this->shader->set_color( &piece->color );

	this->shader->draw( id, piece->face_top, piece->face_num );

	return 0;
}

int mmdpiMesh::set_vertex( const MMDPI_VERTEX* vertex, dword vertex_num )
{
	this->b_vertex_num = vertex_num;
	this->b_vertex = new MMDPI_VERTEX[ b_vertex_num ];
	if( this->b_vertex == 0x00 )
		return -1;
	for( dword i = 0; i < vertex_num; i ++ )
		this->b_vertex[ i ] = vertex[ i ];
	this->shader->set_vertex_buffers( id, this->b_vertex, this->b_vertex_num );
	return 0;
}

int mmdpiMesh::set_face( const mmdpiShaderIndex* face, dword face_num )
{
	this->b_face_num = face_num;
	this->b_face = new mmdpiShaderIndex[ b_face_num ];
	if( this->b_face == 0x00 )
		return -1;
	for( dword i = 0; i < face_num; i ++ )
		this->b_face[ i ] = face[ i ];
	this->shader->set_face_buffers( id, this->b_face, this->b_face_num );
	return 0;
}

int mmdpiMesh::set_material( dword raw_material_id, MMDPI_MATERIAL* raw_material )
{
	this->b_piece = new MMDPI_PIECE();
	if( this->b_piece == 0x00 )
		return -1;

	this->b_piece->raw_material_id	= raw_material_id;
	this->b_piece->raw_material	= raw_material;
	this->b_piece->face_top		= 0;
	this->b_piece->face_num		= b_face_num;
	this->b_piece->has_texture	= 0;
	this->b_piece->is_draw_both	= 0;

	return 0;
}

int mmdpiMesh::set_boneid( const dword* boneid, dword boneid_length )
{
	dword	i = 0;
	this->b_piece->bone_list_num = boneid_length;
	for( ; i < boneid_length; i ++ )
		this->b_piece->bone_list[ i ] = boneid[ i ];
	for( ; i < _MMDPI_MATERIAL_USING_BONE_NUM_; i ++ )
		this->b_piece->bone_list[ i ] = 0;
	return 0;
}

void mmdpiMesh::update_gpu_cache_matrix( MMDPI_BONE_INFO_PTR bone, dword bone_num )
{
	for( uint j = 0; j < this->b_piece->bone_list_num; j ++ )
	{
		int	bone_index = ( int )this->b_piece->bone_list[ j ];
		this->b_piece->matrix[ j ] = bone[ bone_index ].local_matrix;
	}
}

mmdpiMesh::mmdpiMesh( mmdpiShader* shader_p )
{
	this->b_vertex = 0x00;
	this->b_face = 0x00;
	this->b_piece = 0x00;

	this->shader = shader_p;
}

mmdpiMesh::~mmdpiMesh()
{
	delete[] this->b_vertex;
	delete[] this->b_face;
	delete this->b_piece;
}
