//
//	include
//
#include "../mmdpi_include.h"

#pragma once
#include <iostream>

class MMDPI_TGA
{
protected:
	GLuint		bit_length;
	GLubyte*	raw_bits;
	GLubyte*	bits;
	GLenum		format;
	GLuint		internal_format;
	GLuint		width;
	GLuint		height;
	GLuint		bpp;

	GLuint		texture;

public:
	virtual	GLuint	get_id( void );
	virtual int	draw( void );
	int		ReadTGA( const char *filename );
	virtual	GLuint	load (const char *filename );

	MMDPI_TGA();
	~MMDPI_TGA();
};