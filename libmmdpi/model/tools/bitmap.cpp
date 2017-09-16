
//
//　include
//
#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "bitmap.h"


int MMDPI_BMP::ReadBMP( const char *filename )
{
	FILE*			fp;

	BITMAPINFOHEADER	bitmapInfoHeader;
	BITMAPFILEHEADER	header;
	GLubyte			temp = 0;
	

	//　ファイルを開く
	if ( ( fp = fopen( filename, "rb" ) ) == NULL )
	{
		//cout << "Error : Connot open.\n";
		//cout << "File Name : " << filename << endl;
		return -1;
	}

	//　ヘッダー情報の読み取り
	if( fread( &header, sizeof( BITMAPFILEHEADER ), 1, fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}

	//　ファイルチェック
	if( header.bfType != 0x4d42 )
	{
		//cout << "Error : This is not bitmap.\n";
		fclose( fp );
		return -1;
	}

	//　ヘッダー情報の読み取り
	if( fread( &bitmapInfoHeader, sizeof( BITMAPINFOHEADER ), 1, fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}

	//　幅と高さを取得
	width = bitmapInfoHeader.biWidth;
	height = bitmapInfoHeader.biHeight;

	//	bit幅
	bitc = bitmapInfoHeader.biBitCount / 8;

	if( bitmapInfoHeader.biSizeImage == 0 )
		bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * bitc;

	fseek( fp, header.bfOffBits, SEEK_SET );

	//　データサイズを決定し，メモリを確保
	bit_size = bitmapInfoHeader.biSizeImage;
	bits = new GLubyte[ bit_size + 1 ];

	//　エラーチェック
	if( bits == 0x00 )
	{
		//cout << "Error : Allocation error!\n";
		delete[] bits;
		fclose( fp );

		return -1;
	}

	//　ピクセルデータの読み込み
	if( fread( bits, 1, bitmapInfoHeader.biSizeImage, fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}

	//　BGR　→　RGBに変換
	for( uint i = 0; i < bit_size; i += bitc )
	{
		temp = bits[ i + 0 ];
		bits[ i + 0 ] = bits[ i + 2 ];
		bits[ i + 2 ] = temp;
	}
	if( bitc == 4 )
	{
		for( uint i = 0; i < bit_size; i += bitc )
			bits[ i + 3 ] = 1;
	}

	//　ファイルを閉じる
	fclose( fp );

	//	成功
	return 0;
}


int MMDPI_BMP::load( const char *filename )
{
	if( ReadBMP( filename ) )
		return -1;

	internalFormat	= GL_RGB;
	format		= GL_RGB;

	if( bitc == 4 )
	{
		//internalFormat	= GL_RGBA;
		format		= GL_RGBA;
	}

	glGenTextures( 1, &texture );

	//	バインド
	//glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	//	テクスチャの割り当て
	//	gluBuild2DMipmaps( GL_TEXTURE_2D, internalFormat, width, height, format, GL_UNSIGNED_BYTE, bits );
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, bits );  
  
	//	テクスチャの拡大、縮小方法
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

#ifdef _MMDPI_BITMAP_DRAW_SWITCH_
#else
	if( bits )
	{
		delete[] bits;
		bits = NULL;
	}
#endif

	return texture;
}

/*
GLuint	MMDPI_BMP::load( char *file_name )
{
	FILE*	 fp = fopen( file_name, "rb" );
	if( fp == NULL ) 
	{
		printf( "stream did not open.\nFile is %s.\n", file_name );
		return -1;
	}

	fseek( fp, 0x12 , SEEK_SET );
	fread( &width, 4, 1, fp );
	fread( &height, 4, 1, fp );
	bits = new BYTE[ width * height * 3 + 1 ];
	fseek( fp, 0x3E, SEEK_SET );
	fread( bits, 1, width * height * 3, fp );
	fclose( fp );

	//	数を知らせる
    glGenTextures( 1, &texture );
	
	//	テクスチャをバインドします。
	//	テクスチャのバインドとは、指定した名前のテクスチャを有効にする作業を表します。
	//	初めて使われたテクスチャ名が指定された場合は、自動的にオブジェクトが生成されます。
    glBindTexture( GL_TEXTURE_2D, texture );

	// テクスチャ画像はバイト単位に詰め込まれている
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	//　テクスチャの割り当て
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits );
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, bits );

	////	繰り返しで描画
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	//	拡大・縮小についての設定
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );


	//delete[] bits;

	return texture;
}

BYTE* MMDPI_BMP::get_bits( void )
{
	return NULL;
	//return bits;
}

int MMDPI_BMP::draw( void )
{
	if( bits == NULL )
		return -1;

	glBitmap( width, height, 0, 0, 0, 0, bits );
	//	テクスチャ作成
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, bits );
	//glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, bits );

	return 0;
}
*/
int MMDPI_BMP::draw( void )
{
	if( bits == NULL )
		return -1;

//	glBitmap( width, height, 0, 0, 0, 0, bits );
	//	テクスチャ作成
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, bits );
	//glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, bits );

	return 0;
}

GLuint MMDPI_BMP::get_id( void )
{
	return texture;
}

MMDPI_BMP::MMDPI_BMP()
{
	bits = NULL;
	width = 0;
	height = 0;
}

MMDPI_BMP::~MMDPI_BMP()
{
#ifdef _MMDPI_BITMAP_DRAW_SWITCH_
	delete[] bits;
#endif
	glDeleteTextures( 1, &texture );
}
