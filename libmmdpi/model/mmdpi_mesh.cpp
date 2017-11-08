
#include "mmdpi_mesh.h"


int mmdpiMesh::draw( void )
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

	//	GLSL
	//	Input shader infomation

	//	オプション
	option_enable();

	////	裏面描画
	//glDisable( GL_DEPTH_TEST );
	//glCullFace( GL_FRONT );
	//draw_main( 0 );

	//	本処理
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_BACK );
	draw_main( 1 );

	//	オプション
	option_disable();

	return 0;
}

//	描画処理
int mmdpiMesh::draw_main( int cull_flag )
{
	MMDPI_PIECE*		piece = b_piece;
	MMDPI_MATERIAL_PTR	material = piece->raw_material;

	glActiveTexture( GL_TEXTURE0 );

	shader->init_material();
	if( material->texture.get_id() > 0 )
		glBindTexture( GL_TEXTURE_2D, material->texture.get_id() );
	//if( m->texture.get_id() > 0 )
	//	glUniform1f( m->texture.get_id(), 0 );

	//	マテリアルごとに変化する情報を送信
	shader->send_bone( piece );

	shader->set_alpha_for_shader( piece->opacity );

	//	輪郭処理
	if( cull_flag == 0 )
	{
		shader->set_edge_size( piece->edge_size );
		shader->set_edge_color( ( float* )piece->edge_color.ptr() );
	}
	else
		shader->set_edge_size( 0 );

	//	色設定
	shader->set_color( &piece->color );

	//	描画
	shader->draw( id, piece->face_top, piece->face_num );
	
	return 0;
}

int mmdpiMesh::option_enable( void )
{
	//	描画処理
	GLfloat	light_ambient[] = { 0.3f, 0.3f, 0.3f, 1 };
	GLfloat	light_diffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
	GLfloat	light_specular[] = { 0.2f, 0.2f, 0.2f, 1 };


	glEnable( GL_TEXTURE_2D );	//	テクスチャ

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	//	カリング
	//glDisable( GL_CULL_FACE );		//	カリング無効
	glEnable( GL_CULL_FACE );		//	CCWでカリング(反時計回り)
	glFrontFace( GL_CCW );
	glCullFace( GL_FRONT );

	//glEnable( GL_ALPHA_TEST );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	//	GL_SRC_ALPHA : (As/kA,As/kA,As/kA,As/kA)
	//	GL_ONE_MINUS_SRC_ALPHA : (1,1,1,1)-(As/kA,As/kA,As/kA,As/kA)
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA );

	//	テクスチャワープ
	//glEnable( GL_TEXTURE_GEN_S );
	//glEnable( GL_TEXTURE_GEN_T );
	//glEnable( GL_TEXTURE_GEN_R );
	//glEnable( GL_TEXTURE_GEN_Q );

	return 0;
}

int mmdpiMesh::option_disable( void )
{
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );

	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	//glDisable( GL_ALPHA_TEST );

	return 0;
}

int mmdpiMesh::set_vertex( const MMDPI_VERTEX* vertex, dword vertex_num )
{
	b_vertex_num = vertex_num;
	b_vertex = new MMDPI_VERTEX[ b_vertex_num ];
	if( b_vertex == 0x00 )
		return -1;

	for( dword i = 0; i < vertex_num; i ++ )
		b_vertex[ i ] = vertex[ i ];

	shader->set_vertex_buffers( id, b_vertex, b_vertex_num );
	return 0;
}

int mmdpiMesh::set_face( const mmdpiShaderIndex* face, dword face_num )
{
	b_face_num = face_num;
	b_face = new mmdpiShaderIndex[ b_face_num ];
	if( b_face == 0x00 )
		return -1;

	for( dword i = 0; i < face_num; i ++ )
		b_face[ i ] = face[ i ];

	shader->set_face_buffers( id, b_face, b_face_num );
	return 0;
}

int mmdpiMesh::set_material( dword raw_material_id, MMDPI_MATERIAL* raw_material )
{
	b_piece = new MMDPI_PIECE();
	if( b_piece == 0x00 )
		return -1;
	
	b_piece->raw_material_id	= raw_material_id;
	b_piece->raw_material		= raw_material;
	b_piece->face_top		= 0;
	b_piece->face_num		= b_face_num;
	return 0;
}

int mmdpiMesh::set_boneid( const dword* boneid, dword boneid_length )
{
	dword	i;

	b_piece->bone_list_num = boneid_length;
	for( i = 0; i < boneid_length; i ++ )
		b_piece->bone_list[ i ] = boneid[ i ];
	for( ; i < _MMDPI_MATERIAL_USING_BONE_NUM_; i ++ )
		b_piece->bone_list[ i ] = 0;

	return 0;
}

void mmdpiMesh::update_matrix( MMDPI_BONE_INFO_PTR bone, dword bone_num )
{
	for( uint j = 0; j < b_piece->bone_list_num; j ++ )
	{
		int	bone_index = ( int )b_piece->bone_list[ j ];
		b_piece->matrix[ j ] = bone[ bone_index ].local_matrix;
	}
}

mmdpiMesh::mmdpiMesh( mmdpiShader* shader_p )
{
	b_vertex	= 0x00;
	b_face		= 0x00;
	b_piece		= 0x00;

	shader = shader_p;
}

mmdpiMesh::~mmdpiMesh()
{
	delete[] b_vertex;
	delete[] b_face;
	delete b_piece;
}