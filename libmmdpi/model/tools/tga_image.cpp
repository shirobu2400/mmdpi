#include "tga_image.h"


int MMDPI_TGA::ReadTGA( const char *filename )
{
	FILE*		fp;
	GLubyte		header[ 18 ]; 
	GLubyte		pixel_size;
	int		image_type;
	int		result = 0;
	

	//　ファイルを開く
	fp = fopen( filename, "rb" );
	if( fp == 0x00 )
	{	
		//cout << "Error : Connot open!\n";
		//cout << "File Name : " << filename << endl;
		return -1;
	}

	//　ヘッダー情報の読み込み
	if( fread( header, 1, sizeof( header ), fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}

	//	圧縮形式を調査
	image_type = header[ 0x02 ];
	if( image_type == 0x00 )
	{
		fclose( fp );
		return -1;
	}
    
	//　幅と高さを決める
	width	= header[ 13 ] * 0x100 + header[ 12 ];
	height	= header[ 15 ] * 0x100 + header[ 14 ];
    
	//　ビットの深さ
	bpp = header[ 16 ];

	//　24 bit
	if( bpp == 24 )
	{
		internal_format = GL_RGB;
		format		= GL_RGB;
	}
	//　32 bit
	else if( bpp = 32 )
	{
		internal_format = GL_RGBA;
		format		= GL_RGBA;
	}

	//　1ピクセル当たりのバイト数を決定
	pixel_size = bpp / 8;

	//　データサイズの決定
	bit_length = width * height * pixel_size;

	//　メモリを確保
	bits = new GLubyte[ bit_length ];
	if( bits == 0x00 )
	{
		fclose( fp );
		return -1;
	}

	raw_bits = new GLubyte[ bit_length ];
	if( raw_bits == 0x00 )
	{
		fclose( fp );
		return -1;
	}

	//　テクセルデータを一気に読み取り
	if( fread( raw_bits, 1, bit_length, fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}

	//　BGR(A)をRGB(A)にコンバート
	if( image_type == 0x02 || image_type == 0x03 )
	{
		for( uint i = 0; i < bit_length; i += pixel_size )
		{
			bits[ i + 0 ] = raw_bits[ i + 2 ];
			bits[ i + 1 ] = raw_bits[ i + 1 ];
			bits[ i + 2 ] = raw_bits[ i + 0 ];
			if( pixel_size == 4 )
				bits[ i + 3 ] = raw_bits[ i + 3 ];
		}
	}
	else if( image_type == 0x09 || image_type == 0x0a )
	{
		GLubyte*	elements = new GLubyte[ pixel_size ];
		if( elements == 0x00 )
			return -1;
		
		//	ラングレンス圧縮(RLE)
		for( uint d = 0, o = 0; d < bit_length; )
		{
			int	packet = raw_bits[ o ++ ] & 0xff;

			if( ( packet & 0x80 ) != 0x00 )
			{
				for( uint j = 0; j < pixel_size; j ++ )
					elements[ j ] = raw_bits[ o ++ ];
				int	count = ( packet & 0x7f ) + 1;
				for( int k = 0; k < count; k ++ )
					for( uint j = 0; j < pixel_size; j ++ )
						bits[ d ++ ] = elements[ j ];
			}
			else
			{
				int	count = ( packet + 1 ) * pixel_size;
				for( int k = 0; k < count; k ++ )
					bits[ d ++ ] = raw_bits[ o ++ ];
			}
		}

		delete[] elements;
	}	
	else
	{
		result = -1;
	}

	//　ファイルを閉じる
	fclose( fp );

	delete[] raw_bits;

	return result;
}

GLuint MMDPI_TGA::load( const char *filename )
{
	//　ファイル読み込み
	if( ReadTGA( filename ) )
		return -1;

	//　テクスチャを生成
	glGenTextures( 1, &texture );

	//　テクスチャをバインド
	//glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	//　
	//if( bpp == 24 ) 
	//	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	//else if( bpp == 32 ) 
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	//　テクスチャの割り当て
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, bits );
	
	////　テクスチャを拡大・縮小する方法の指定
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  
	//　テクスチャ環境
	//   glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	//　メモリ解放
	if( raw_bits )
	{
		delete[] bits;
		bits = 0x00;
	}

	return texture;
}

GLuint MMDPI_TGA::get_id( void )
{
	return texture;
}

int MMDPI_TGA::draw( void )
{
	if( texture == 0 )
		return -1;

	//	テクスチャ作成
	//glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, bits );

	return 0;
}

MMDPI_TGA::MMDPI_TGA()
{
	texture = 0;
}

MMDPI_TGA::~MMDPI_TGA()
{
	glDeleteTextures( 1, &texture );
}
