
#include "../mmdpi_struct.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef struct my_error_mgr 
{
	struct jpeg_error_mgr jerr;
	jmp_buf jmpbuf;
} my_error_mgr;

static void error_exit( j_common_ptr cinfo ) 
{
	my_error_mgr *err =  (my_error_mgr* )cinfo->err;
	( *cinfo->err->output_message )( cinfo );
	longjmp( err->jmpbuf, 1 );
}

GLuint MMDPI_JPG::load( const char *filename )
{
	uint32_t			x, y;
	struct jpeg_decompress_struct	jpegd;
	my_error_mgr			myerr;
	unsigned char*			image = 0x00;
	JSAMPROW			buffer = 0x00;
	JSAMPROW			row;
	int				stride;
	int				color = 3;

	FILE	*fp;
	fp = fopen( filename, "rb" );
	if( fp == 0x00 )
		return -1;

	jpegd.err = jpeg_std_error( &myerr.jerr );
	myerr.jerr.error_exit = error_exit;
	if( setjmp( myerr.jmpbuf ) )
		return -1;
	jpeg_create_decompress( &jpegd );
	jpeg_stdio_src( &jpegd, fp );
	if( jpeg_read_header( &jpegd, TRUE ) != JPEG_HEADER_OK ) 
		return -1;
	jpeg_start_decompress( &jpegd );
	if( jpegd.out_color_space != JCS_RGB ) 
		return -1;
	stride = jpegd.output_width * jpegd.output_components;
	buffer = new JSAMPLE[ stride ];
	if( ( buffer = ( JSAMPROW )calloc( stride, 1 ) ) == 0x00 )
		return -1;
	image = new unsigned char[ jpegd.output_height * jpegd.output_width * color ];
	if( image == 0x00 )
		return -1;
	for( y = 0; y < jpegd.output_height; y ++ ) 
	{
		jpeg_read_scanlines( &jpegd, &buffer, 1 );
		row = buffer;
		for( x = 0; x < jpegd.output_width; x ++ ) 
		{
			image[ ( ( jpegd.output_height - y - 1 ) * jpegd.output_width + x ) * color + 0 ] = *row ++;
			image[ ( ( jpegd.output_height - y - 1 ) * jpegd.output_width + x ) * color + 1 ] = *row ++;
			image[ ( ( jpegd.output_height - y - 1 ) * jpegd.output_width + x ) * color + 2 ] = *row ++;
		}
	}
	jpeg_finish_decompress( &jpegd );
	jpeg_destroy_decompress( &jpegd );
	delete[] buffer;
	fclose( fp );

	//	Add image	
	uint		format = GL_RGB;
	uint		internal_format = GL_RGB;
	GLsizei		width = jpegd.output_width;
	GLsizei		height = jpegd.output_height;

	glGenTextures( 1, &texture );

	//	バインド
	//glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	//	テクスチャの拡大、縮小方法
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );


	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image ); 
	delete[] image;

	return texture;
}