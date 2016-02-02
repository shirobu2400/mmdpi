
#include "../h/mmdpi_shader.h"


const int _send_gpu_data_num_ = 4;

int mmdpiShader::shader_on( void )
{
	if( program )
	{
		glUseProgram( program );
	}
	else
	{
		GLint	length;
		GLchar*	log;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
		log = new GLchar[ length ];
		glGetShaderInfoLog( bone_size_id, length, &length, log );
		fprintf( stderr, "Linked log = \"%s\"", log );
		delete[] log;
		return -1;
	}
	return 0;
}

void mmdpiShader::shader_off( void )
{
	glUseProgram( 0 );
}

int mmdpiShader::default_shader( void )
{ 
	static GLchar vertex_shader_src[] = 
#if defined( _MMDPI_OPENGL_ES_DEFINES_ ) || defined( _MMDPI_PRIJECTION_MATRIX_SELF_ )
	"uniform   mat4		ProjectionMatrix;\n"
#else
	"mat4		ProjectionMatrix = gl_ModelViewProjectionMatrix;\n"
#endif
	"attribute	vec3		Vertex;\n"
	"attribute	vec3		Normal;\n"
	"attribute	vec4		g_Uv;\n"
	"varying	vec4		v_Uv;\n"
	"attribute	vec4		BoneWeights;	//	頂点ウェイト\n"
	"uniform	float		Edge_size;		//	エッジサイズ\n"
	"uniform	vec4		Edge_color;		//	エッジカラー\n"
	"attribute	vec3		SkinVertex;\n"
	"\n"
	"//	Bone Info\n"
	"//	ボーン姿勢情報\n"
	"uniform mat4		BoneMatrix[ 24 ];\n"
	"attribute vec4		BoneIndices;	//	ボーンインデックス\n"
	"\n"
	"varying   vec4		v_color;\n"
	"\n"
	"// 頂点シェーダメイン関数\n"
	"void main( void )\n"
	"{\n"
	"	mat4	skinTransform;\n"
	"	vec3	vertex00;\n"
	"\n"
	"	skinTransform = mat4( 0 );\n"
	"\n"
	"	float weight[ 4 ];\n"
	"	weight[ 0 ] = BoneWeights.x;\n"
	"	weight[ 1 ] = BoneWeights.y;\n"
	"	weight[ 2 ] = BoneWeights.z;\n"
	"	weight[ 3 ] = BoneWeights.w;\n"
	"\n"
	"	float indices[ 4 ];\n"
	"	indices[ 0 ] = BoneIndices.x;\n"
	"	indices[ 1 ] = BoneIndices.y;\n"
	"	indices[ 2 ] = BoneIndices.z;\n"
	"	indices[ 3 ] = BoneIndices.w;\n"
	"\n"
	"	for( int i = 0; i < 4; i ++ )\n"
	"	{\n"
	"		int bone_index = int( indices[ i ] + 0.1 );\n"
	"		skinTransform += weight[ i ] * BoneMatrix[ bone_index ];\n"
	"	}\n"
	"\n"
#ifdef _MMDPI_OUTLINE_
	"	vertex00 = Vertex + SkinVertex + Normal * Edge_size * 0.05;\n"
#else
	"	vertex00 = Vertex;\n"
#endif
	"	gl_Position = ProjectionMatrix * skinTransform * vec4( vertex00, 1 );\n"
	"	v_Uv = g_Uv;\n"
	"	v_color = vec4( 1.0, 1.0, 1.0, 0.0 );"
#ifdef _MMDPI_OUTLINE_
	"	if( Edge_size > 0.00001 )\n"
	"		v_color = Edge_color;//vec4( 0.0, 0.0, 0.0, 1.0 );\n"
#endif
	"}\n"
	"\n"
	;

	static GLchar fragment_shader_src[] = 
	"//precision lowp 	float;\n"
	"\n"
	"uniform	sampler2D	Tex01;\n"
	"uniform	float		TexToonFlag;\n"
	"uniform	sampler2D	TexToon;\n"
	"varying	vec4		v_Uv;\n"
	"\n"
	"varying	vec4		v_color;\n"
	"uniform	float		Alpha;\n"
	"\n"
	"void main( void )\n"
	"{\n"
	"	vec4	color  = texture2D( Tex01, v_Uv.xy );\n"
	"	color.a = color.a * Alpha;\n"
	"	if( TexToonFlag > 0.5 )\n"
	"	{\n"
	"		vec4	tc = texture2D( TexToon, v_Uv.xy );\n"
	"		tc.a = 1.0;\n"
	"		color = color * tc;\n"
	"	}\n"
	"	gl_FragColor = color * ( 1.0 - v_color.a ) + v_color * v_color.a;\n"
	"}\n"
	;

#ifndef _MMDPI_OPENGL_ES_DEFINES_
	glewInit();
#endif
	program = glCreateProgram();

	int		result = create_shader( GL_VERTEX_SHADER, vertex_shader_src );
	result = create_shader( GL_FRAGMENT_SHADER, fragment_shader_src ) || result;

	//	Link
	result = result || link();

	if( result == 0 )
		shader_setting();

	return result; 
}

void mmdpiShader::shader_setting( void )
{
	//	設定
	shader_on();

	//	マトリックス
	pm_id = glGetUniformLocation( program, ( GLchar* )"ProjectionMatrix" );

	//	頂点
	vertex_id = glGetAttribLocation( program, ( GLchar* )"Vertex" );
	glEnableVertexAttribArray( vertex_id );

	//	法線
	normal_id = 0;
#ifdef _MMDPI_OUTLINE_
	normal_id = glGetAttribLocation( program, ( GLchar* )"Normal" );
	glEnableVertexAttribArray( normal_id );
#endif

	//	UV
	uv_id = glGetAttribLocation( program, ( GLchar* )"g_Uv" );
	glEnableVertexAttribArray( uv_id );

	//	基本的なテクスチャ
	tex2d_01_id = glGetUniformLocation( program, ( GLchar* )"Tex01" );

	//	アルファ
	alpha_id = glGetUniformLocation( program, ( GLchar* )"Alpha" );
	
	//	テクスチャサイズ
	bone_size_id = glGetUniformLocation( program, ( GLchar* )"BoneTextureSize" );
	
	// Attribute1にindices変数、Attribute2にweights変数を割り当てる
	bone_weights_id = glGetAttribLocation( program, ( GLchar* )"BoneWeights" );
	glEnableVertexAttribArray( bone_weights_id );
	
	bone_indices_id = glGetAttribLocation( program, ( GLchar* )"BoneIndices" );
	glEnableVertexAttribArray( bone_indices_id );

	bone_matrix_id = glGetUniformLocation( program, ( GLchar* )"BoneMatrix" );

	tex2d_toon_id = glGetUniformLocation( program, ( GLchar* )"TexToon" );		
	glUniform1f( tex2d_toon_id, 0 );

	tex2d_toon_flag = glGetUniformLocation( program, ( GLchar* )"TexToonFlag" );	
	glUniform1f( tex2d_toon_flag, 0 );


	edge_size_id = glGetUniformLocation( program, ( GLchar* )"Edge_size" );
	glUniform1f( edge_size_id, 0 );

	edge_color_id = glGetUniformLocation( program, ( GLchar* )"Edge_color" );
	glUniform1f( edge_color_id, 0 );

	//	スキン
	skinvertex_id = glGetAttribLocation( program, ( GLchar* )"SkinVertex" );
	glEnableVertexAttribArray( skinvertex_id );
	
	//	Uniform Variable
	glActiveTexture( GL_TEXTURE0 );
	glUniform1i( tex2d_01_id, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glUniform1i( tex2d_toon_id, 1 );

	shader_off();
}

int mmdpiShader::create_shader( GLuint shader_type, const GLchar* src )
{
	int		shader_size = strlen( ( const char * )src );

	shader = glCreateShader( shader_type );
	glShaderSource( shader, 1, ( const GLchar ** )&src, &shader_size );
	glCompileShader( shader );

	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if( compiled == 0 )
	{
		GLint	length;
		GLchar*	log;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
		log = new GLchar[ length ];
		glGetShaderInfoLog( shader, length, &length, log );
		fprintf( stderr, "Compiled log = \"%s\"", log );
		delete[] log;
	}

	glAttachShader( program, shader );
	glDeleteShader( shader );
	
	return 0;
}

int mmdpiShader::link( void )
{
	if( program )
		glLinkProgram( program );
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
	if( !linked ) 
	{ // error
		GLint infoLen = 0;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &infoLen );
		if( infoLen > 0 ) 
		{
			char* infoLog = new char[ infoLen ];
			glGetProgramInfoLog( program, infoLen, NULL, infoLog );
			printf( "Error linking program:\n%s\n", infoLog );
			delete[] ( infoLog );
		}
		glDeleteProgram( program );
		return GL_FALSE;
	}
	glGetProgramiv( program, GL_LINK_STATUS, &linked_fragment );
	glGetProgramiv( program, GL_LINK_STATUS, &linked_vertex );

	return 0;
}

void mmdpiShader::set_vertex_buffers( int buffer_id, MMDPI_VERTEX_PTR a_vertex_p, dword vertex_num )
{
	this->vertex_num = vertex_num;

	//	Buffer 
	if( buffers.size() <= ( uint )buffer_id )
	{
		buffers.push_back( new mmdpiShaderBuffer() );
		if( buffers.size() <= ( uint )buffer_id )
		{
			perror( "Buffer error!!" );
			return ;
		}
	}
	
	/* バッファオブジェクトの名前を作る */
	glBindBuffer( GL_ARRAY_BUFFER, buffers[ buffer_id ]->get_vertex() );

	glBufferData( GL_ARRAY_BUFFER, sizeof( MMDPI_VERTEX ) * vertex_num, a_vertex_p, GL_STATIC_DRAW );
}

void mmdpiShader::set_face_buffers( int buffer_id, ushort* face_p, dword face_num )
{
	//	Face
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ buffer_id ]->get_face() );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( ushort ) * face_num, face_p, GL_STATIC_DRAW );
}

void mmdpiShader::set_buffer( int buffer_id )
{
	dword		vertex_start = 0;
	
	glBindBuffer( GL_ARRAY_BUFFER, buffers[ buffer_id ]->get_vertex() );
	
	glVertexAttribPointer( vertex_id, 3, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( vertex_start ) );

	glVertexAttribPointer( uv_id, 4, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( vertex_start + sizeof( mmdpiVector3d ) ) );

	glVertexAttribPointer( bone_indices_id, 4, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( vertex_start + sizeof( mmdpiVector3d ) + sizeof( mmdpiVector4d ) ) );

	glVertexAttribPointer( bone_weights_id, 4, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( vertex_start + sizeof( mmdpiVector3d ) + sizeof( mmdpiVector4d ) * 2 ) );

#ifdef _MMDPI_OUTLINE_
	glVertexAttribPointer( normal_id, 3, GL_FLOAT, GL_FALSE, 
		sizeof( MMDPI_VERTEX ), ( const void * )( vertex_start + sizeof( mmdpiVector3d ) + sizeof( mmdpiVector4d ) * 3 ) );
#endif
	
#ifdef _MMDPI_USINGSKIN_
	//	スキン
	glVertexAttribPointer( skinvertex_id, 3, GL_FLOAT, GL_FALSE, 
		sizeof( MMDPI_VERTEX ), ( const void * )( vertex_start + sizeof( mmdpiVector3d ) * 2 + sizeof( mmdpiVector4d ) * 3 ) );
#endif

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void mmdpiShader::init_material( void )
{
	glUniform1f( tex2d_toon_flag, 0 );
}

void mmdpiShader::draw( int buffer_id, dword fver_num_base, dword face_num )
{
	//if( now_buffer_id != buffer_id )
	
	set_buffer( buffer_id );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ buffer_id ]->get_face() );
	
	now_buffer_id = buffer_id;

	glDrawElements( GL_TRIANGLES, face_num, GL_UNSIGNED_SHORT, ( const void * )( sizeof( ushort ) * fver_num_base ) );
}

void mmdpiShader::send_material_info( MMDPI_MATERIAL_PTR material )
{
	GLfloat		matrix[ _MMDPI_MATERIAL_USING_BONE_NUM_ ][ 16 ] = { 0 };

	for( uint i = 0; i < material->bone_list_num; i ++ )
	{
		for( int j = 0; j < 16; j ++ )
			matrix[ i ][ j ] = material->matrix[ i ][ j ];
	}

	glUniformMatrix4fv( bone_matrix_id, material->bone_list_num, GL_FALSE, ( GLfloat * )matrix );
}

void mmdpiShader::set_projection_matrix( mmdpiMatrix_ptr projection_matrix )
{
	if( pm_id >= 0 )
		glUniformMatrix4fv( pm_id, 1, GL_FALSE, ( const GLfloat* )projection_matrix );
}

void mmdpiShader::set_alpha_for_shader( GLfloat alpha )
{
	glUniform1f( alpha_id, alpha );
}

void mmdpiShader::set_edge_size( float edge )
{
#ifdef _MMDPI_OUTLINE_
	glUniform1f( edge_size_id, edge );
#endif
}

void mmdpiShader::set_edge_color( float* edge_color )
{
#ifdef _MMDPI_OUTLINE_
	float	def_edge_color[ 4 ] = { 0, 0, 0, 1 };
		
	if( edge_color == 0x00 )
		edge_color = def_edge_color;

	glUniform4f( edge_color_id, edge_color[ 0 ], edge_color[ 1 ], edge_color[ 2 ], edge_color[ 3 ] );
#endif
}


mmdpiShader::mmdpiShader()
{
	now_buffer_id = 0;

	vertex_id	= 0;
	normal_id	= 0;
	uv_id		= 0;
	bone_weights_id	= 0;
	bone_indices_id	= 0;
	skinvertex_id	= 0;

	program = 0;
}

mmdpiShader::~mmdpiShader()
{
	for( uint i = 0; i < buffers.size(); i ++ )
		delete buffers[ i ];

	if( vertex_id > 0 )
		glDisableVertexAttribArray( vertex_id );
#ifdef _MMDPI_OUTLINE_
	if( normal_id > 0 )
		glDisableVertexAttribArray( normal_id );
#endif
	if( uv_id > 0 )
		glDisableVertexAttribArray( uv_id );

	if( bone_weights_id > 0 )
		glDisableVertexAttribArray( bone_weights_id );
	if( bone_indices_id > 0 )
		glDisableVertexAttribArray( bone_indices_id );
#ifdef _MMDPI_USINGSKIN_
	if( skinvertex_id > 0 )
		glDisableVertexAttribArray( skinvertex_id );
#endif
	if( program )
		glDeleteProgram( program );
}