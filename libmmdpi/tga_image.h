//
//・ｽ@include
//
#include "mmdpi_include.h"

#pragma once
#include <iostream>

//////////////////////////////////////////////////////////////////////////
//・ｽ@・ｽ@TGALoader class
//////////////////////////////////////////////////////////////////////////
class MMDPI_TGA
{
protected:

    GLuint		imageSize;
    GLubyte*	imageData;
    GLenum		format;
    GLuint		internalFormat;
    GLuint		width;
    GLuint		height;
    GLuint		bpp;

    GLuint		texture;

public:

	virtual	GLuint	get_id( void );
	virtual int		draw( void );
    int				ReadTGA( const char *filename );
    virtual	GLuint	load (const char *filename );

    MMDPI_TGA();
    ~MMDPI_TGA();

};