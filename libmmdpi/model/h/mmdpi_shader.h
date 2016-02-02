
#include "mmdpi_struct.h"

#pragma once

class mmdpiShaderBuffer
{
private:

	//	VBO
	GLuint			face_buffers;
	GLuint			vertex_buffers;
	GLuint			buffers[ 2 ];
		
public:
	GLuint	get_vertex( void )
	{
		return vertex_buffers;
	}
	GLuint	get_face( void )
	{
		return face_buffers;
	}
	GLuint*	get_buffers( void )
	{
		return buffers;
	}
	mmdpiShaderBuffer()
	{
		glGenBuffers( 2, buffers );
		vertex_buffers	= buffers[ 0 ];
		face_buffers	= buffers[ 1 ];
	}
	~mmdpiShaderBuffer()
	{
		glDeleteBuffers( 2, buffers );
	}
};

class mmdpiShader
{
private :

	//	Program
	GLuint		shader;
	GLuint		program;
	GLint		compiled;
	GLint		linked_vertex;
	GLint		linked_fragment;
	GLint		linked;
	
	//	ProjectionMatrix
	GLint		pm_id;

    //      頂点ID
	GLint		vertex_id;
	GLint		uv_id;
	GLint		normal_id;
	GLint		skinvertex_id;

	//	テクスチャID
	GLint		tex2d_01_id;
	GLint		tex2d_toon_id;
	GLint		tex2d_toon_flag;

	//	アルファID
	GLint		alpha_id;

	//	エッジID
	GLint		edge_size_id;
	GLint		edge_color_id;

	//	頂点テクスチャ
	GLint		bone_size_id;
	GLint		bone_weights_id;
	GLint		bone_indices_id;
	GLint		bone_matrix_id;

	//	VBO
	vector<mmdpiShaderBuffer*>	buffers;

	void		shader_setting( void );

	int			now_buffer_id;

	dword		vertex_num;

public :

	int			shader_on( void );
	void		shader_off( void );
	
	int			default_shader( void );
	int			create_shader( GLuint shader_type, const GLchar* src );
	int			link( void );

	void		set_vertex_buffers( int buffer_id, MMDPI_VERTEX_PTR a_vertex_p, dword vertex_num );
	void		set_face_buffers( int buffer_id, ushort* face_p, dword face_num );
		
	void		set_edge_size( float edge );
	void		set_edge_color( float* edge_cloor );
	void		send_material_info( MMDPI_MATERIAL_PTR material );
	void		set_alpha_for_shader( GLfloat alpha );
	
	void		set_buffer( int buffer_id );
	void		set_projection_matrix( mmdpiMatrix_ptr projection_matrix );
	void		init_material( void );

	void		draw( int buffer_id, dword fver_num_base, dword face_num );

	mmdpiShader();
	~mmdpiShader();

} ;