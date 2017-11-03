
#ifdef __cplusplus
extern "C"
{
#endif

#	include <stdio.h>
#	include <stdlib.h>
#	include <math.h>
#	include <GLES2/gl2.h>
#	include <EGL/egl.h>
#	include "bcm_host.h"
#	include <sys/time.h>
#	include <time.h>

#ifdef __cplusplus
}
#endif

//int 	usec = 16 * 1000;//1000 / 30 * 1000 ;	//	30 fps
int		_fps_ = 60;
uint32_t	_screen_max_size_ = 0x1000;//1280;	//640;

extern "C"
{

	typedef struct
	{
		EGLNativeWindowType  nativeWin;
		EGLDisplay  display;
		EGLContext  context;
		EGLSurface  surface;
		EGLint      majorVersion;
		EGLint      minorVersion;
		int         width;
		int         height;
	} ScreenSettings;

	typedef struct
	{
		float m[ 16 ];
	} Mat4;

	typedef struct
	{
		GLint   aPosition;
		GLint   aTex;
		GLint   uVpMatrix;
		GLint   uModelMatrix;
		Mat4    VpMatrix;
	} ShaderParams;

	ShaderParams    g_sp;
	ScreenSettings  g_sc;
}

#include "mmdpi.h"
mmdpi*		p = 0x00;
mmdpiMatrix	Model_offset;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>


class Fps
{
	static const int	sample_average	= 60;		//平均を取るサンプル数

	float			start_time;         //測定開始時刻
	int			count;				//カウンタ
	float			m_fps;				//fps
	int			fps;				//設定したFPS

public:

	Fps()
	{
		start_time = get_time();
		count = 0;
		m_fps = 30;
		fps = 30;
	}

	float get_fps( void )
	{
		return fps;
	}

	float get_time( void )
	{
		//	高精度時刻計測
		struct timespec tp ;
		clock_gettime( CLOCK_MONOTONIC, &tp );
		return ( float )( tp.tv_sec * 1000.0 + ( double )tp.tv_nsec / 1000000.0 );	//	milli sec
		//return glutGet( GLUT_ELAPSED_TIME );
		//return clock() * 1000.0f / ( float )CLOCKS_PER_SEC;
	}

	void set_fps( int fps )
	{
		if( fps < 1 )
			return ;
		this->fps = fps;
		m_fps = fps;
	}

	bool update( void )
	{
		if( count == 0 )	//	1フレーム目なら時刻を記憶
			start_time = get_time();

		if( count == sample_average )	//	60フレーム目なら平均を計算する
		{
			float		t = get_time();
			if( t < 1e-16f )
				m_fps = 30.0 / fps;
			else
				m_fps = 1000.0f / ( ( t - start_time ) / ( float )sample_average );
			count = 0;
			start_time = t;
		}
		count ++;

		return true;
	}

	float get_mfps( void )
	{
		return m_fps;
	}

	void draw( void )
	{
		if( count < 2 )
		{
			printf( "%f[ step ]\n", m_fps );
		}
	}

	float get_wait_time( void )
	{
		float		took_time = get_time() - start_time;	//かかった時間
		float		wait_time = count * 1000.0f / fps - took_time;	//待つべき時間
		if( wait_time > 0 )
			return wait_time / 1000.0f;	//待機
		return 1.0f / fps;
	}
};

Fps* 			fps = 0x00;
static int		vmd_flag = 0;


char* get_command_option( const char* option, int argc, char* argv[] )
{
	int			i;
	size_t			option_length = strlen( option );

	for( i = 0; i < argc; i ++ )
	{
		if( strncmp( argv[ i ], option, option_length ) == 0 )
		{
			char*	r = argv[ i ] + option_length;
			if( *r )
				return r;
			if( i + 1 < argc )
				return argv[ i + 1 ];
			return argv[ i ];
		}
	}

	return 0x00;
}

void init( int argc, char *argv[] )
{
	char*	model_name = 0x00;
	model_name = get_command_option( "-p", argc, argv );
	if( model_name && model_name[ 0 ] )
	{
		p = new mmdpi();
		if( p->load( model_name ) )
		{
			printf( "Not found %s.\n", model_name );
			exit( 0 );
		}
	}

	char*	vmd_name = 0x00;
	vmd_name = get_command_option( "-v", argc, argv );
	vmd_flag = 0;
	if( p && vmd_name && vmd_name[ 0 ] )
	{
		if( p->vmd_load( vmd_name ) )
		{
			printf( "Not found %s.\n", vmd_name );
			exit( 0 );
		}
		else
			vmd_flag = 1;
	}

	if( p )
	{
		p->set_fps( _fps_ );
		char*	fps_name = 0x00;
		fps_name = get_command_option( "-f", argc, argv );
		if( fps_name )
		{
			_fps_ = atoi( fps_name );
			p->set_fps( _fps_ );
		}
	}

	fps = new Fps();
	fps->set_fps( _fps_ );

	char*	sound_name = 0x00;
	sound_name = get_command_option( "-s", argc, argv );
	if( sound_name )
	{
		int	i, j;
		char	cmd_string[ 255 ] = { 0 };
		int	length = 0;
		strcpy( cmd_string, sound_name );
		puts( cmd_string );
		length = strlen( cmd_string );
		cmd_string[ length ] = ' ';
		cmd_string[ length + 1 ] = '&';
		cmd_string[ length + 2 ] = '\0';

		system( cmd_string );
	}

	Model_offset.rotation( 0, 1, 0, 3.14f );

	puts( "End Loading." );
}

void draw()
{
	fps->update();
	if( p )
	{
		if( p->get_vmd( 0 ) )
		{
			float	frame = 30.0f / fps->get_mfps();
			//	フレームを進める関数
			//（MMD は１秒間に３０フレームがデフォルト）
			//	60fpsで実行の場合、0.5frame ずつフレームにたいしてモーションを進める
			( *p->get_vmd( 0 ) ) += frame;
		}
		p->set_bone_matrix( 0, Model_offset );
		p->draw();
	}
}

void end()
{
	delete p;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

char get_keyboard( void )
{
	int f = open( "/dev/tty", O_RDONLY|O_NONBLOCK|O_NDELAY|O_NOCTTY );
	char c;
	struct termios term, default_term;

	//	non cannonical mode
	tcgetattr( fileno( stdin ), &default_term );
	term.c_lflag &= ~ICANON;
	tcsetattr( fileno( stdin ), TCSANOW, &term );

	if( read( f, &c, 1 ) == 0 )
			c = 0;
	close( f );

	tcsetattr( 0x00, TCSANOW, &default_term );

	return c;
}

EGLBoolean WinCreate(ScreenSettings *sc)
{
	uint32_t success = 0;
	uint32_t width;
	uint32_t height;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	static EGL_DISPMANX_WINDOW_T nativewindow;
	VC_DISPMANX_ALPHA_T alpha = {DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, 0};

	success = graphics_get_display_size(0, &width, &height);
	if( success < 0 )
		return EGL_FALSE;

	if( _screen_max_size_ > 0 )
	{
		if( width > _screen_max_size_ )
			width = _screen_max_size_;
		if( height > _screen_max_size_ )
			height = _screen_max_size_;
	}

	sc->width = width;
	sc->height = height;

	vc_dispmanx_rect_set(&dst_rect, 0, 0, sc->width, sc->height);
	vc_dispmanx_rect_set(&src_rect, 0, 0, sc->width << 16, sc->height << 16);

	dispman_display = vc_dispmanx_display_open(0);
	dispman_update = vc_dispmanx_update_start(0);
	dispman_element = vc_dispmanx_element_add( dispman_update, dispman_display,
	0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, &alpha, 0, ( DISPMANX_TRANSFORM_T )0);

	vc_dispmanx_update_submit_sync(dispman_update);
	nativewindow.element = dispman_element;
	nativewindow.width = width;
	nativewindow.height = height;
	sc->nativeWin = &nativewindow;

	return EGL_TRUE;
}

EGLBoolean SurfaceCreate( ScreenSettings *sc )
{
	EGLint attrib[] =
	{
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     24,
		EGL_NONE
	};
	EGLint context[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	EGLint numConfigs;
	EGLConfig config;

	sc->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if( sc->display == EGL_NO_DISPLAY )
		return EGL_FALSE;
	if( !eglInitialize(sc->display, &sc->majorVersion, &sc->minorVersion ) )
		return EGL_FALSE;
	if( !eglChooseConfig( sc->display, attrib, &config, 1, &numConfigs ) )
		return EGL_FALSE;

	sc->surface = eglCreateWindowSurface( sc->display, config, sc->nativeWin, NULL );
	if( sc->surface == EGL_NO_SURFACE )
		return EGL_FALSE;
	sc->context = eglCreateContext( sc->display, config, EGL_NO_CONTEXT, context );
	if( sc->context == EGL_NO_CONTEXT )
		return EGL_FALSE;
	if( !eglMakeCurrent( sc->display, sc->surface, sc->surface, sc->context ) )
		return EGL_FALSE;

	return EGL_TRUE;
}

void makeUnit( Mat4 *m )
{
	memset(m, 0, sizeof(Mat4));
	m->m[0] = m->m[5] = m->m[10]= m->m[15] = 1.0f;
}

void makeProjectionMatrix( Mat4 *m, float n, float f, float hfov, float r )
{
	float w = 1.0f / tan(hfov * 0.5f * M_PI / 180);
	float h = w * r;
	float q = 1.0f / (f - n);

	m->m[0] = w;
	m->m[5] = h;
	m->m[10]= -(f + n) * q;
	m->m[11]= -1.0f;
	m->m[14]= -2.0f * f * n * q;
	m->m[1] = m->m[2] = m->m[3] = m->m[4]  = m->m[6]  = m->m[7]
		= m->m[8] = m->m[9] = m->m[12] = m->m[13] = m->m[15] = 0.0f;
}

/*
** 透視投影行列
*/
void perspectiveMatrix(float left, float right,
                       float top, float bottom,
                       float near, float far,
                       Mat4 *matrix4)
{
	float dx = right - left;
	float dy = bottom - top;
	float dz = far - near;

	GLfloat* matrix = matrix4->m;

	matrix[ 0] =  2.0f * near / dx;
	matrix[ 5] =  2.0f * near / dy;
	matrix[ 8] =  (right + left) / dx;
	matrix[ 9] =  (top + bottom) / dy;
	matrix[10] = -(far + near) / dz;
	matrix[11] = -1.0f;
	matrix[14] = -2.0f * far * near / dz;
	matrix[ 1] = matrix[ 2] = matrix[ 3] = matrix[ 4] =
	matrix[ 6] = matrix[ 7] = matrix[12] = matrix[13] = matrix[15] = 0.0f;
}

void setPosition(Mat4 *m, float x, float y, float z)
{
	m->m[12] = x;
	m->m[13] = y;
	m->m[14] = z;
}

void setRotationX(Mat4 *m, float degree)
{
	float rad = ((float)degree * M_PI / 180.0);
	m->m[ 5] = cos(rad);
	m->m[ 9] = - sin(rad);
	m->m[ 6] = sin(rad);
	m->m[10] = cos(rad);
}

void setRotationY(Mat4 *m, float degree)
{
	float rad = ((float)degree * M_PI / 180.0);
	m->m[ 0] = cos(rad);
	m->m[ 8] = sin(rad);
	m->m[ 2] = - sin(rad);
	m->m[10] = cos(rad);
}

void setRotationZ(Mat4 *m, float degree)
{
	float rad = ((float)degree * M_PI / 180.0);
	m->m[ 0] = cos(rad);
	m->m[ 4] = - sin(rad);
	m->m[ 1] = sin(rad);
	m->m[ 5] = cos(rad);
}

void mulMatrix(Mat4 *r, Mat4 *a, Mat4 *b)
{
	float a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15;
	float b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15;

	a0 =a->m[ 0]; a1 =a->m[ 1]; a2 =a->m[ 2]; a3 =a->m[ 3];
	a4 =a->m[ 4]; a5 =a->m[ 5]; a6 =a->m[ 6]; a7 =a->m[ 7];
	a8 =a->m[ 8]; a9 =a->m[ 9]; a10=a->m[10]; a11=a->m[11];
	a12=a->m[12]; a13=a->m[13]; a14=a->m[14]; a15=a->m[15];
	b0 =b->m[ 0]; b1 =b->m[ 1]; b2 =b->m[ 2]; b3 =b->m[ 3];
	b4 =b->m[ 4]; b5 =b->m[ 5]; b6 =b->m[ 6]; b7 =b->m[ 7];
	b8 =b->m[ 8]; b9 =b->m[ 9]; b10=b->m[10]; b11=b->m[11];
	b12=b->m[12]; b13=b->m[13]; b14=b->m[14]; b15=b->m[15];

	r->m[ 0] = a0 * b0 + a4 * b1 +  a8 * b2 + a12 * b3;
	r->m[ 1] = a1 * b0 + a5 * b1 +  a9 * b2 + a13 * b3;
	r->m[ 2] = a2 * b0 + a6 * b1 + a10 * b2 + a14 * b3;
	r->m[ 3] = a3 * b0 + a7 * b1 + a11 * b2 + a15 * b3;
	r->m[ 4] = a0 * b4 + a4 * b5 +  a8 * b6 + a12 * b7;
	r->m[ 5] = a1 * b4 + a5 * b5 +  a9 * b6 + a13 * b7;
	r->m[ 6] = a2 * b4 + a6 * b5 + a10 * b6 + a14 * b7;
	r->m[ 7] = a3 * b4 + a7 * b5 + a11 * b6 + a15 * b7;
	r->m[ 8] = a0 * b8 + a4 * b9 +  a8 * b10+ a12 * b11;
	r->m[ 9] = a1 * b8 + a5 * b9 +  a9 * b10+ a13 * b11;
	r->m[10] = a2 * b8 + a6 * b9 + a10 * b10+ a14 * b11;
	r->m[11] = a3 * b8 + a7 * b9 + a11 * b10+ a15 * b11;
	r->m[12] = a0 * b12+ a4 * b13+  a8 * b14+ a12 * b15;
	r->m[13] = a1 * b12+ a5 * b13+  a9 * b14+ a13 * b15;
	r->m[14] = a2 * b12+ a6 * b13+ a10 * b14+ a14 * b15;
	r->m[15] = a3 * b12+ a7 * b13+ a11 * b14+ a15 * b15;
}

int print_mat4( Mat4* m )
{
	printf(
		"%f, %f, %f, %f\n"
		"%f, %f, %f, %f\n"
		"%f, %f, %f, %f\n"
		"%f, %f, %f, %f\n"
		,
		m->m[ 0 ], m->m[ 1 ], m->m[ 2 ], m->m[ 3 ],
		m->m[ 4 ], m->m[ 5 ], m->m[ 6 ], m->m[ 7 ],
		m->m[ 8 ], m->m[ 9 ], m->m[ 10 ], m->m[ 11 ],
		m->m[ 12 ], m->m[ 13 ], m->m[ 14 ], m->m[ 15 ]
	);

	return 0;
}

int main( int argc, char *argv[] )
{
	unsigned int	frames = 0;
	int		res;
	Mat4		projection_matrix;

	Mat4		viewMat;
	Mat4		rotMat;
	Mat4		modelMat;
	float		aspect;
	float		dw, dh;

	int		debug_flag = 0;

	if( argc < 2 )
	{
		printf(
			"argment: -p [pmd or pmx file name] 	\n"
			"argment: -v [vmd file name] 		\n"
			"argment: -d				\n"
		);
		return 0;
	}

	bcm_host_init();
	res = WinCreate( &g_sc );
	if( !res )
	{
		printf( "Create window error!\n" );
		return 0;
	}
	res = SurfaceCreate( &g_sc );
	if( !res )
	{
		printf( "Create surface error!\n" );
		return 0;
	}

	makeUnit( &viewMat );

	aspect = ( float )g_sc.width / ( float )g_sc.height;

	//makeProjectionMatrix(&g_sp.VpMatrix, 1, 65536, 53, aspect);
	//setPosition(&viewMat, 0, -4, -24 );

	dw = 0.5f * aspect;
	dh = 0.5f;

	perspectiveMatrix( -dw, +dw, -dh, +dh, 1, 160, &g_sp.VpMatrix );
	setPosition( &viewMat, 0, -12, -32 );

	//print_mat4( &g_sp.VpMatrix );

	mulMatrix( &g_sp.VpMatrix, &g_sp.VpMatrix, &viewMat );

	makeUnit( &modelMat );

	mulMatrix( &projection_matrix, &modelMat, &g_sp.VpMatrix );

	makeUnit( &rotMat );
	setRotationY( &rotMat, 0.5 ); /* 30 degree/sec */

	init( argc, argv );

	if( get_command_option( "-d", argc, argv ); )
		debug_flag = 1;

	//print_mat4( &projection_matrix );

	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	Mat4	delta_mat;
	makeUnit( &delta_mat );

	glViewport(0, 0, g_sc.width, g_sc.height);

	/* 1200frame / 60fps = 20sec */
	while( !p->get_vmd( 0 ) || !p->get_vmd( 0 )->is_end() )
	{
		Mat4	pl_matrix;
		Mat4	delta_key_mat;
		int	key_matrix_flag = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* X Rotation */
		mulMatrix( &pl_matrix, &projection_matrix, &delta_mat );
		//glUniformMatrix4fv(g_sp.uModelMatrix, 1, GL_FALSE, modelMat.m);

		p->set_projection_matrix( pl_matrix.m );
		draw();

		char c = get_keyboard();
		makeUnit( &delta_key_mat );
		switch( c )
		{
		case 'w':	setPosition( &delta_key_mat, 0, 0, +1 ); key_matrix_flag = 1;	break;
		case 's':	setPosition( &delta_key_mat, 0, 0, -1 ); key_matrix_flag = 1;	break;
		case 'a':	setRotationY( &delta_key_mat, +2.0 ); key_matrix_flag = 1;	break;
		case 'd':	setRotationY( &delta_key_mat, -2.0 ); key_matrix_flag = 1;	break;
		}
		if( key_matrix_flag )
			mulMatrix( &delta_mat, &delta_mat, &delta_key_mat );

		if( c == 'q' )
			break;

		eglSwapBuffers(g_sc.display, g_sc.surface);
		frames ++;

		//glutTimerFunc( fps->get_wait_time() * 1000.0f, timer, 0 );
		usleep( 1700 );
		if( debug_flag )
			fps->draw();
	}

	printf( "Ending process!\n" );

	end();

	return 0;
}
