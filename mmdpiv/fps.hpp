
#include <stdio.h>

class Fps
{
	static const int	sample_average	= 60;		//���ς����T���v����
	
	float			start_time;         //����J�n����
	int			count;				//�J�E���^
	float			m_fps;				//fps
	int			fps;				//�ݒ肵��FPS

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
		return glutGet( GLUT_ELAPSED_TIME );
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
		if( count == 0 )	//	1�t���[���ڂȂ玞�����L��
			start_time = get_time();

		if( count == sample_average )	//	60�t���[���ڂȂ畽�ς��v�Z����
		{ 
			float		t = get_time();
			if( t < 1e-8f )
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
			printf( "%f[ fps ]\n", m_fps );	
		}
	}

	float get_wait_time( void )
	{
		float		took_time = get_time() - start_time;	//������������
		float		wait_time = count * 1000.0f / fps - took_time;	//�҂ׂ�����
		if( wait_time > 0 )
			return wait_time / 1000.0f;	//�ҋ@
		return 1.0f / fps;
	}
};