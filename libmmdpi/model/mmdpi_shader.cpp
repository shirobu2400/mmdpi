
#include "mmdpi_shader.hpp"


const int _send_gpu_data_num_ = 4;

int mmdpiShader::shader_on( void )
{
	if( this->program )
		glUseProgram( this->program );
	else
	{
		GLint	length;
		GLchar*	log;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
		log = new GLchar[ length ];
		if( log == 0x00 )
			return -1;
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
	static GLchar vertex_shader_src[ 0x1000 ] = { 0 };
	sprintf( vertex_shader_src,
#if defined( _MMDPI_OPENGL_ES_DEFINES_ ) || defined( _MMDPI_PRIJECTION_MATRIX_SELF_ )
	"uniform   mat4			ProjectionMatrix;\n"
#else
	"mat4				ProjectionMatrix = gl_ModelViewProjectionMatrix;\n"
#endif
	"attribute	vec3		Vertex;		\n"
#ifdef _MMDPI_OUTLINE_
	"attribute	vec3		Normal;		\n"
	"uniform	float		Edge_size;	\n"		// エッジサイズ
	"uniform	vec4		Edge_color;	\n"		// エッジカラー
#endif
	"attribute	vec4		gUv;		\n"
	"varying	vec4		vUv;		\n"
	"attribute	vec3		SkinVertex;	\n"
	"\n"
	//"	Bone Info\n"
	//"ボーン姿勢情報\n"
	"uniform	mat4		BoneMatrix[ %d ];\n"
	"attribute	vec4		BoneWeights;	\n"	// 頂点ウェイト
	"attribute	vec4		BoneIndices;	\n"	// ボーンインデックス
	"\n"
	"uniform	vec4		gColor;		\n"
	"varying	vec4		Color;		\n"
	"\n"
	// 頂点シェーダメイン関数
	"void main( void )\n"
	"{\n"
	"	mat4	skinTransform;\n"
	"	vec3	vertex00;\n"
	"\n"
	"\n"
	"\n"
	"	skinTransform  = BoneWeights[ 0 ] * BoneMatrix[ int( BoneIndices[ 0 ] ) ];\n"
	"	skinTransform += BoneWeights[ 1 ] * BoneMatrix[ int( BoneIndices[ 1 ] ) ];\n"
	"	skinTransform += BoneWeights[ 2 ] * BoneMatrix[ int( BoneIndices[ 2 ] ) ];\n"
	"	skinTransform += BoneWeights[ 3 ] * BoneMatrix[ int( BoneIndices[ 3 ] ) ];\n"
	"\n"
#ifdef _MMDPI_OUTLINE_
	"	vertex00 = Vertex + SkinVertex + Normal * Edge_size * 0.02;\n"
#else
	"	vertex00 = Vertex;\n"
#endif
	"	gl_Position = ProjectionMatrix * skinTransform * vec4( vertex00, 1 );\n"
	"	vUv = gUv;\n"
	"	Color = gColor;\n"
	//"	Color = vec4( 1.0, 1.0, 1.0, 0.0 );\n"
#ifdef _MMDPI_OUTLINE_
	"	if( Edge_size > 0.00001 )\n"
	"		Color = Edge_color;//vec4( 0.0, 0.0, 0.0, 1.0 );\n"
#endif
	"}\n"
	"\n", _MMDPI_MATERIAL_USING_BONE_NUM_ );

	static GLchar fragment_shader_src[] =
	"//precision lowp 	float;\n"
	"\n"
	"uniform	sampler2D	Tex01;\n"
	"uniform	float		TexToonFlag;\n"
	"uniform	sampler2D	TexToon;\n"
	"varying	vec4		vUv;\n"
	"\n"
	"varying	vec4		Color;\n"
	"uniform	float		Alpha;\n"
	"\n"
	"void main( void )\n"
	"{\n"
	"	vec4	color = texture2D( Tex01, vUv.xy );\n"
	"	color.a = color.a * Alpha;\n"
	//"	if( TexToonFlag > 0.5 )\n"
	//"	{\n"
	//"		vec4	tc = texture2D( TexToon, vUv.xy );\n"
	//"		tc.a = 1.0;\n"
	//"		color = color * tc;\n"
	//"	}\n"
	"	gl_FragColor = color * ( 1.0 - Color.a ) + Color * ( Color.a );\n"
	//"	gl_FragColor = color;\n"
	"}\n"
	;

	int	result;
	GLenum	err;
#ifndef _MMDPI_OPENGL_ES_DEFINES_
	err = glewInit();
	if( err != GLEW_OK )
	{
		std::cout << "GLEW Error : " << glewGetErrorString( err ) << std::endl;
		return -1;
	}
	if( GLEW_VERSION_2_1 != GL_TRUE )
	{
		std::cout << "GLEW_VERSION_2_1 Error : " << std::endl;
		std::cout << "  Glew Ver. " << glewGetString(GLEW_VERSION) << std::endl;
		std::cout << "  OpenGL Ver. " << glGetString(GL_VERSION) << "(<= 2.0)" << std::endl;
		return -1;
	}
#endif

	this->program = glCreateProgram();
	if( this->program == 0 )
	{
		puts( "glCreateProgram Error." );
		return result;
	}

	result = create_shader( GL_VERTEX_SHADER, vertex_shader_src );
	if( result )
	{
		puts( "GL_VERTEX_SHADER Error." );
		return result;
	}
	result = create_shader( GL_FRAGMENT_SHADER, fragment_shader_src );
	if( result )
	{
		puts( "GL_FRAGMENT_SHADER Error." );
		return result;
	}

	// Link
	result = this->link();
	if( result )
	{
		puts( "GLSL link Error." );
		return result;
	}

	shader_setting();
	return result;
}

void mmdpiShader::shader_setting( void )
{
	// 設定
	shader_on();

	// マトリックス
	pm_id = glGetUniformLocation( this->program, ( GLchar* )"ProjectionMatrix" );

	// 頂点
	vertex_id = glGetAttribLocation( this->program, ( GLchar* )"Vertex" );
	glEnableVertexAttribArray( vertex_id );

	// UV
	uv_id = glGetAttribLocation( this->program, ( GLchar* )"gUv" );
	glEnableVertexAttribArray( uv_id );

	// 法線
#ifdef _MMDPI_OUTLINE_
	normal_id = glGetAttribLocation( this->program, ( GLchar* )"Normal" );
	glEnableVertexAttribArray( normal_id );
#endif
	// 色
	color_id = glGetUniformLocation( this->program, ( GLchar* )"gColor" );
	mmdpiColor	color;
	set_color( &color );

	// 基本的なテクスチャ
	tex2d_01_id = glGetUniformLocation( this->program, ( GLchar* )"Tex01" );

	// アルファ
	alpha_id = glGetUniformLocation( this->program, ( GLchar* )"Alpha" );

	// テクスチャサイズ
	bone_size_id = glGetUniformLocation( this->program, ( GLchar* )"BoneTextureSize" );

	// Attribute1にindices変数、Attribute2にweights変数を割り当てる
	bone_weights_id = glGetAttribLocation( this->program, ( GLchar* )"BoneWeights" );
	glEnableVertexAttribArray( bone_weights_id );

	bone_indices_id = glGetAttribLocation( this->program, ( GLchar* )"BoneIndices" );
	glEnableVertexAttribArray( bone_indices_id );

	bone_matrix_id = glGetUniformLocation( this->program, ( GLchar* )"BoneMatrix" );

	tex2d_toon_id = glGetUniformLocation( this->program, ( GLchar* )"TexToon" );
	glUniform1i( tex2d_toon_id, 0 );

	tex2d_toon_flag = glGetUniformLocation( this->program, ( GLchar* )"TexToonFlag" );
	glUniform1f( tex2d_toon_flag, 0 );

	edge_size_id = glGetUniformLocation( this->program, ( GLchar* )"Edge_size" );
	glUniform1f( edge_size_id, 0 );

	edge_color_id = glGetUniformLocation( this->program, ( GLchar* )"Edge_color" );
	glUniform4f( edge_color_id, 0, 0, 0, 0 );

	// スキン
	skinvertex_id = glGetAttribLocation( this->program, ( GLchar* )"SkinVertex" );
	glEnableVertexAttribArray( skinvertex_id );

	// Uniform Variable
	glActiveTexture( GL_TEXTURE0 );
	glUniform1i( tex2d_01_id, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glUniform1i( tex2d_toon_id, 1 );

	shader_off();
}

// シェーダ生成
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

	glAttachShader( this->program, shader );
	glDeleteShader( shader );

	return 0;
}

// シェーダリンク
int mmdpiShader::link( void )
{
	glLinkProgram( this->program );
	glGetProgramiv( this->program, GL_LINK_STATUS, &linked );
	if( linked == 0 )
	{ // error
		GLint	infoLen = 0;
		glGetProgramiv( this->program, GL_INFO_LOG_LENGTH, &infoLen );
		if( infoLen > 0 )
		{
			char* infoLog = new char[ infoLen ];
			glGetProgramInfoLog( this->program, infoLen, 0x00, infoLog );
			printf( "Error linking this->program:\n%s\n", infoLog );
			delete[] infoLog;
		}
		glDeleteProgram( this->program );
		return -1;
	}
	glGetProgramiv( this->program, GL_LINK_STATUS, &linked_fragment );
	glGetProgramiv( this->program, GL_LINK_STATUS, &linked_vertex );

	return 0;
}

// シェーダバッファに頂点データを設定
void mmdpiShader::set_vertex_buffers( int buffer_id, MMDPI_VERTEX_PTR a_vertex_p, dword vertex_num )
{
	this->vertex_num = vertex_num;

	// Buffer
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

// シェーダバッファに面データを設定
void mmdpiShader::set_face_buffers( int buffer_id, mmdpiShaderIndex* face_p, dword face_num )
{
	// Face
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ buffer_id ]->get_face() );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( mmdpiShaderIndex ) * face_num, face_p, GL_STATIC_DRAW );
}

// シェーダバッファにデータ領域を設定
void mmdpiShader::set_buffer( int buffer_id )
{
	dword		vertex_start	= 0;


	glBindBuffer( GL_ARRAY_BUFFER, buffers[ buffer_id ]->get_vertex() );

// vertex
	glVertexAttribPointer( vertex_id, 3, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( ( uintptr_t )vertex_start ) );
	vertex_start += sizeof( mmdpiVector3d );

// uv
	glVertexAttribPointer( uv_id, 4, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( ( uintptr_t )vertex_start ) );
	vertex_start += sizeof( mmdpiVector4d );

// bone index
	glVertexAttribPointer( bone_indices_id, 4, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( ( uintptr_t )vertex_start ) );
	vertex_start += sizeof( mmdpiVector4d );

// bone weight
	glVertexAttribPointer( bone_weights_id, 4, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( ( uintptr_t )vertex_start ) );
	vertex_start += sizeof( mmdpiVector4d );

#ifdef _MMDPI_OUTLINE_
	// 法線ベクトル
	glVertexAttribPointer( normal_id, 3, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( ( uintptr_t )vertex_start ) );
	vertex_start += sizeof( mmdpiVector4d );
#endif

#ifdef _MMDPI_USINGSKIN_
	// スキン
	glVertexAttribPointer( skinvertex_id, 3, GL_FLOAT, GL_FALSE,
		sizeof( MMDPI_VERTEX ), ( const void * )( ( uintptr_t )vertex_start ) );
	vertex_start += sizeof( mmdpiVector3d );
#endif

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void mmdpiShader::init_material( void )
{
	glUniform1f( tex2d_toon_flag, 0 );
}

void mmdpiShader::draw( int buffer_id, dword fver_num_base, dword face_num )
{
	set_buffer( buffer_id );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ buffer_id ]->get_face() );

	now_buffer_id = buffer_id;

	glDrawElements( GL_TRIANGLES, face_num, GL_UNSIGNED_SHORT, ( const void * )( sizeof( mmdpiShaderIndex ) * fver_num_base ) );
	//glDrawElements( GL_TRIANGLES, face_num, GL_UNSIGNED_INT, ( const void * )( sizeof( mmdpiShaderIndex ) * fver_num_base ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void mmdpiShader::send_bone( MMDPI_PIECE* piece )
{
	GLfloat		matrix[ _MMDPI_MATERIAL_USING_BONE_NUM_ ][ 16 ] = { 0 };

	for( uint i = 0; i < piece->bone_list_num; i ++ )
	{
		for( int j = 0; j < 16; j ++ )
			matrix[ i ][ j ] = piece->matrix[ i ][ j ];
	}

	glUniformMatrix4fv( bone_matrix_id, piece->bone_list_num, GL_FALSE, ( GLfloat * )matrix );
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

void mmdpiShader::set_color( mmdpiColor* color )
{
	glUniform4f( color_id, color->r, color->g, color->b, color->a );
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
	color_id	= 0;
	uv_id		= 0;
	bone_weights_id	= 0;
	bone_indices_id	= 0;
	skinvertex_id	= 0;

	this->program = 0;
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
	if( this->program )
		glDeleteProgram( this->program );
}
