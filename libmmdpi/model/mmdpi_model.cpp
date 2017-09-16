
#include "mmdpi_model.h"


int mmdpiModel::create( void )
{
	//	シェーダの作成
	if( mmdpiShader::default_shader() )
		return -1;
	
	//	シェーダに設定
	for( dword i = 0; i < mmdpiAdjust::get_face_num(); i ++ )
	{
		MMDPI_BLOCK_FACE_PTR	face = &get_face_block()[ i ];
		mmdpiShader::set_vertex_buffers	( i, face->vertex, face->vertex_num );
		mmdpiShader::set_face_buffers	( i, face->face  , face->face_num   );
	}

	//	bullet
	phy_load_flag = 0;
	if( bullet_flag == 0 )
		return 0;

	//	成功
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
	update_matrix( mmdpiBone::bone, mmdpiBone::bone_num );

	//	Using My Shader
	shader_on();	

	//	ProjectioMatrix
	mmdpiShader::set_projection_matrix( &projection_matrix );
	
	//	GLSL
	//	Input shader infomation
	
	//	オプション
	OptionEnable();

	//	裏面描画
	glDisable( GL_DEPTH_TEST );
	glCullFace( GL_FRONT );
	draw_main( 0 );
	
	//	本処理
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_BACK );
	draw_main( 1 );

	//	オプション
	OptionDisable();

	//	Not Using My Shader
	shader_off();

	return ;
}

//	描画処理
int mmdpiModel::draw_main( int cull_flag )
{
	//	輪郭処理
	for( dword f = 0; f < get_face_num(); f ++ )
	{
		MMDPI_BLOCK_FACE_PTR	face = &get_face_block()[ f ];
		for( uint i = 0; i < face->material_num; i ++ )
		{
			MMDPI_MATERIAL_PTR	m = face->material[ i ];
			uint			material_hash = m->pid;

			glActiveTexture( GL_TEXTURE0 );
			
			init_material();
			if( m->texture.get_id() > 0 )
				glBindTexture( GL_TEXTURE_2D, m->texture.get_id() );
			//if( m->texture.get_id() > 0 )
			//	glUniform1f( m->texture.get_id(), 0 );

			//	マテリアルごとに変化する情報を送信
			send_material_info( m );

			set_alpha_for_shader( m->opacity );
		
			//	輪郭処理
			if( cull_flag == 0 )
			{
				set_edge_size( m->edge_size );
				set_edge_color( ( float* )m->edge_color.ptr() );
			}
			else
				set_edge_size( 0 );

			mmdpiShader::draw( face->buffer_id, m->face_top, m->face_num );
		}
	}
	return 0;
}

int mmdpiModel::OptionEnable( void )
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

int mmdpiModel::OptionDisable( void )
{
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
	
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	//glDisable( GL_ALPHA_TEST );
	
	return 0; 
}

void mmdpiModel::set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix )
{
	mmdpiBone::set_bone_matrix( bone_index, matrix );
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
	fps = 30;
}