
#include "../mmdpi_struct.h"


GLuint MMDPI_PNG::load( const char *filename )
{
	// ファイルを開く
	FILE	*fp;
	fp = fopen( filename, "rb" );
	if( fp == 0x00 )
		return -1;
	png_structp	pPng;
	png_infop	pInfo;
	dword		dwWidth, dwHeight;
	int		pixel_depth, color_type, nInterlaceType;


	// ここからファイルを読み込み
	pPng = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0x00, 0x00, 0x00 );
	if( pPng == 0x00 )
		return -1;
	pInfo = png_create_info_struct( pPng );
	if( pInfo == 0x00 )
		return -1;
	png_init_io( pPng, fp );
	png_read_info( pPng, pInfo );
	png_get_IHDR( pPng, pInfo, ( png_uint_32* )&dwWidth, ( png_uint_32* )&dwHeight, 
			&pixel_depth, &color_type, &nInterlaceType, 0x00, 0x00 );
	// ここまでで必要な情報が一通り取得できます

	uint	width	= dwWidth;
	uint	height	= dwHeight;
			
	// メモリ領域確保
	int	rb		= png_get_rowbytes( pPng, pInfo );
	BYTE	color_num	= rb / width;

	//	Gray scale
	if( color_num == 1 )
		return -1;

	BYTE*	data		= new BYTE[ height * rb ];
	BYTE**	recv		= new BYTE*[ height ];

	if( data == 0x00 || recv == 0x00 )
		return -1;
	
	for( uint i = 0; i < height; i ++ )
		recv[ i ] = &data[ ( height - i - 1 ) * rb ];
	png_read_image( pPng, recv );

	png_read_end( pPng, pInfo );

	png_destroy_read_struct( &pPng, &pInfo, 0x00 );

	// ファイルを閉じる
	fclose( fp );

	glGenTextures( 1, &texture );

	//	バインド
	//glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	//	テクスチャの拡大、縮小方法
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	//	テクスチャの割り当て
	uint	format		= GL_RGBA;
	uint	internal_format	= GL_RGBA;
	if( color_num == 3 )
	{
		format		= GL_RGB;
		internal_format = GL_RGB;
	}
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data );  
  
	delete[] data;
	delete[] recv;

	return texture;
}