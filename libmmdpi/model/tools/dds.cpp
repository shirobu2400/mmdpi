
#include "dds.h"


int MMDPI_DDS::readFile( const char *filename )
{
	FILE*			fp;
	long			image_cur;
	long			image_end;
	int			block_size = 16;
	
	// ファイルを開く
	fp = fopen( filename, "rb" );
	if( fp == 0x00 )
	{
		//cout << "Error : Connot open.\n";
		//cout << "File Name : " << filename << endl;
		return -1;
	}
	
	// ヘッダ読み込み
	fread( &this->head, sizeof( MMDPI_DDS_HEADER ), 1, fp );

	// マジックの検査
	if( strncmp( this->head.magic, "DDS ", 4 ) != 0 || this->head.size != 124 )
		return -1;

	//this->size   = this->head.size;
	this->flag   = this->head.flag;
	
	this->width  = this->head.width;
	this->height = this->head.height;
	this->depth  = this->head.depth;

	// フォーマット判別
	if( this->head.pixel_format.flag & MMDPI_DDPF_FOURCC )
	{
		GLubyte*		buffer;

		switch( this->head.pixel_format.fourCC )
		{
		case MMDPI_DDS_FOURCC_DXT1:
			this->internal_format	= GL_RGB;
			this->format		= GL_RGB;
			block_size		= 8;

			break;
		case MMDPI_DDS_FOURCC_DXT3:
			this->internal_format	= GL_RGBA;
			this->format		= GL_RGBA;
			block_size		= 16;

			break;
		case MMDPI_DDS_FOURCC_DXT5:
			this->internal_format	= GL_RGBA;
			this->format		= GL_RGBA;
			block_size		= 16;

			break;
		default:
			return -1;
		}

		// テクセルデータのサイズを算出
		image_cur = ftell( fp );
		fseek( fp, 0, SEEK_END );
		image_end = ftell( fp );
		fseek( fp, image_cur, SEEK_SET );
		this->size = image_end - image_cur;
		this->bits = new GLubyte[ this->height * this->width * 4 ];

		buffer = new GLubyte[ this->size ];

		// ピクセルデータの読み込み
		fread( buffer, sizeof( GLubyte ), this->size, fp );

		// 並び変え
		int		px = 0;
		int		py = 0;
		GLubyte*	p = buffer;

		for( uint i = 0; i < this->size; i += 0x40 )
		{
			int	image_index;

			// ブロックを取得
			ushort	b0 = *( ( ushort* )p + 0x00 );
			ushort	b1 = *( ( ushort* )p + 0x02 );
			ushort	b2 = *( ( ushort* )p + 0x04 );
			ushort	b3 = *( ( ushort* )p + 0x06 );
			
			GLubyte	c0_r = ( ( b0 >> 11 ) & 0x1f ) * 8;
			GLubyte	c0_g = ( ( b0 >>  5 ) & 0x3f ) * 4;
			GLubyte	c0_b = ( ( b0 >>  0 ) & 0x1f ) * 8;

			GLubyte	c1_r = ( ( b1 >> 11 ) & 0x1f ) * 8;
			GLubyte	c1_g = ( ( b1 >>  5 ) & 0x3f ) * 4;
			GLubyte	c1_b = ( ( b1 >>  0 ) & 0x1f ) * 8;

			dword	color_bits = ( b2 | ( b3 << 0x10 ) );

			dword	color[ 4 ] = { 0 };

			color[ 0 ] = ( ( c0_r << 0 ) | ( c0_g << 8 ) | ( c0_b << 16 ) );
			color[ 1 ] = ( ( c1_r << 0 ) | ( c1_g << 8 ) | ( c1_b << 16 ) );
			color[ 2 ] = ( ( c0_r << 0 ) | ( c0_g << 8 ) | ( c0_b << 16 ) );
			color[ 3 ] = ( ( c1_r << 0 ) | ( c1_g << 8 ) | ( c1_b << 16 ) );

			// 配置
			for( int y = 0; y < 4; y ++ )
			{
				for( int x = 0; x < 4; x ++ )
				{
					int	color_index = ( color_bits & 0x03 );

					image_index = ( ( py + y ) * this->width + ( px + x ) ) * 4;
					this->bits[ image_index + 0 ] = ( GLubyte )( color[ color_index ] >>  0 );
					this->bits[ image_index + 1 ] = ( GLubyte )( color[ color_index ] >>  8 );
					this->bits[ image_index + 2 ] = ( GLubyte )( color[ color_index ] >> 16 );
					this->bits[ image_index + 3 ] = 255;//( GLubyte )( color[ color_index ] >> 24 );

					color_bits >>= 2;
				}
			}

			px += 4;
			if( ( int )this->width <= px )
			{
				py += 4;
				px = 0;
				if( ( int )this->height <= py )
					break;
			}
			p += 0x40;
		}
		
		delete[] buffer;
	}
	else if( this->head.pixel_format.flag & ( MMDPI_DDPF_RGB | MMDPI_DDPF_ALPHAPIXELS | MMDPI_DDPF_ALPHA | MMDPI_DDPF_BUMPDUDV | MMDPI_DDPF_LUMINANCE ) )
	{
		GLubyte*	buffer;

		// 非圧縮
		// Bitmask による定義
		dword		bit_count	= this->head.pixel_format.bpp;
		dword		r_mask		= this->head.pixel_format.red_mask;
		dword		g_mask		= this->head.pixel_format.green_mask;
		dword		b_mask		= this->head.pixel_format.blue_mask;
		dword		a_mask		= this->head.pixel_format.alpha_mask;

		dword		r_shiftn	= 0;
		dword		g_shiftn	= 0;
		dword		b_shiftn	= 0;
		dword		a_shiftn	= 0;

		int		color_size	= bit_count / 8;

		dword		r, g, b, a;

		// シフトサイズを計算
		r = r_mask;
		g = g_mask;
		b = b_mask;
		a = a_mask;

		while( r && ( r & 0xff ) == 0x00 )
		{
			r >>= 8;
			r_shiftn += 8;
		}
		while( g && ( g & 0xff ) == 0x00 )
		{
			g >>= 8;
			g_shiftn += 8;
		}
		while( b && ( b & 0xff ) == 0x00 )
		{
			b >>= 8;
			b_shiftn += 8;
		}
		while( a && ( a & 0xff ) == 0x00 )
		{
			a >>= 8;
			a_shiftn += 8;
		}

		// テクセルデータのサイズを算出
		image_cur = ftell( fp );
		fseek( fp, 0, SEEK_END );
		image_end = ftell( fp );
		fseek( fp, image_cur, SEEK_SET );
		this->size = image_end - image_cur;
		this->bits = new GLubyte[ this->size ];
		if( this->bits == 0x00 )
			return -1;

		buffer = new GLubyte[ this->size ];
		if( buffer == 0x00 )
			return -1;

		// ピクセルデータの読み込み
		fread( buffer, sizeof( GLubyte ), this->size, fp );

		// マスクと比較し色を取得
		for( dword i = 0; i < this->size; i += color_size )
		{
			GLubyte*	p = &buffer[ i ];
			dword		color = *( dword* )p;
			
			r = ( r_mask & color ) >> r_shiftn;
			g = ( g_mask & color ) >> g_shiftn;
			b = ( b_mask & color ) >> b_shiftn;
			a = ( a_mask & color ) >> a_shiftn;

			this->bits[ i + 0 ] = r;
			this->bits[ i + 1 ] = g;
			this->bits[ i + 2 ] = b;
			if( color_size == 4 )
				this->bits[ i + 3 ] = a;
		}

		// 上下反転
		for( int j = 0; j < ( signed )this->height; j ++ )
		{
			for( int i = 0; i < ( signed )this->width; i ++ )
			{
				int	i1 = i;
				int	j1 = this->height - j - 1;

				for( int c = 0; c < color_size; c ++ )
					buffer[ ( j1 * this->width + i1 ) * color_size + c ] = this->bits[ ( j * this->width + i ) * color_size + c ];	
			}
		}
		for( dword i = 0; i < this->size; i ++ )
			this->bits[ i ] = buffer[ i ];

		if( color_size == 4 )
		{
			this->internal_format	= GL_RGBA;
			this->format		= GL_RGBA;
		}
		else
		{
			this->internal_format	= GL_RGB;
			this->format		= GL_RGB;
		}

		delete[] buffer;
	}
	else
		return -1; // error

	// ファイルを閉じる
	fclose( fp );

	return 0;
}

int MMDPI_DDS::load( const char *filename )
{
	// 一応初期化
	this->internal_format	= GL_RGBA;
	this->format		= GL_RGBA;

	if( this->readFile( filename ) )
		return -1;

	glGenTextures( 1, &this->texture );

	// バインド
	//glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, this->texture );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// テクスチャの割り当て
	glTexImage2D( GL_TEXTURE_2D, 0, this->internal_format, this->width, this->height, 0, this->format, GL_UNSIGNED_BYTE, this->bits );  
  
	// テクスチャの拡大、縮小方法
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	if( this->bits )
	{
		delete[] this->bits;
		this->bits = 0x00;
	}

	return this->texture;
}

GLuint MMDPI_DDS::get_id( void )
{
	return this->texture;
}

uint MMDPI_DDS::get_width( void )
{
	return this->width;
}

uint MMDPI_DDS::get_height( void )
{
	return this->height;
}

MMDPI_DDS::MMDPI_DDS()
{
	this->texture = 0;
}

MMDPI_DDS::~MMDPI_DDS()
{
	glDeleteTextures( 1, &this->texture );
}
