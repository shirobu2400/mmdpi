#include "tga_image.h"


int MMDPI_TGA::ReadTGA( const char *filename )
{
	FILE		*fp;
	GLubyte		header[ 18 ]; 
	GLubyte		bytePerPixel;
	GLuint		temp;

	//　ファイルを開く
	if( ( fp = fopen( filename, "rb" ) ) == NULL )
	{
		cout << "Error : Connot open!\n";
		cout << "File Name : " << filename << endl;
		return -1;
	}

	//　ヘッダー情報の読み込み
	if( fread( header, 1, sizeof( header ), fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}
    
	//　幅と高さを決める
	width = header[ 13 ] * 256 + header[ 12 ];
	height = header[ 15 ] * 256 + header[ 14 ];
    
	//　ビットの深さ
	bpp = header[ 16 ];

	//　24 bit
	if( bpp == 24 )
	{
		format = GL_RGB;
		internalFormat = GL_RGB;
	}
	//　32 bit
	else if( bpp = 32 )
	{
		format = GL_RGBA;
		internalFormat = GL_RGBA;
	}

	//　1ピクセル当たりのバイト数を決定
	bytePerPixel = bpp / 8;

	//　データサイズの決定
	imageSize = width * height * bytePerPixel;

	//　メモリを確保
	imageData = new GLubyte[ imageSize ];

	//　テクセルデータを一気に読み取り
	if( fread( imageData, 1, imageSize, fp ) == 0 )
	{
		fclose( fp );
		return -1;
	}

	//　BGR(A)をRGB(A)にコンバート
	for( uint i = 0; i < imageSize; i += bytePerPixel )
	{
		temp = imageData[ i ];
		imageData[ i ] = imageData[ i + 2 ];
		imageData[ i + 2 ] = temp;
	}

	//　ファイルを閉じる
	fclose( fp );

	return 0;
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
	if( bpp == 24 ) 
		glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	else if( bpp == 32 ) 
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	//　テクスチャの割り当て
	//gluBuild2DMipmaps( GL_TEXTURE_2D, internalFormat, width, height, format, GL_UNSIGNED_BYTE, imageData );
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, imageData );

	////　テクスチャを拡大・縮小する方法の指定
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  
	//　テクスチャ環境
	//   glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	//　メモリ解放
	if( imageData )
	{
		delete[] imageData;
		imageData = NULL;
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
