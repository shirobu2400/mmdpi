
#include <stdio.h>

class Fps
{
	static const int	sample_average	= 60;		//平均を取るサンプル数
	
	float				start_time;         //測定開始時刻
	int					count;				//カウンタ
	float				m_fps;				//fps
	int					fps;				//設定したFPS

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
		if( count == 0 )	//	1フレーム目なら時刻を記憶
			start_time = get_time();

		if( count == sample_average )	//	60フレーム目なら平均を計算する
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
		float		took_time = get_time() - start_time;	//かかった時間
		float		wait_time = count * 1000.0f / fps - took_time;	//待つべき時間
		if( wait_time > 0 )
			return wait_time / 1000.0f;	//待機
		return 1.0f / fps;
	}
};