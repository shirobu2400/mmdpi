
#include "../../h/mmdpi_include.h"

//#define _MMDPI_BITMAP_DRAW_SWITCH_

#pragma once

#ifndef _WINDOWS_

#pragma pack( push, 1 )	//アラインメント制御をオフる

typedef struct tagBITMAPFILEHEADER 
{
  ushort			 bfType;
  dword			 bfSize;
  ushort           bfReserved1;
  ushort           bfReserved2;
  dword          bfOffBits;
} BITMAPFILEHEADER ;

typedef struct tagBITMAPCOREHEADER
{
    dword          bcSize;
    SHORT          bcWidth;
    SHORT          bcHeight;
    ushort           bcPlanes;
    ushort           bcBitCount;
} BITMAPCOREHEADER ;

typedef struct tagBITMAPINFOHEADER
{
    dword          biSize;
    INTEGER        biWidth;
    INTEGER        biHeight;
    ushort           biPlanes;
    ushort           biBitCount;
    dword          biCompression;
    dword          biSizeImage;
    INTEGER        biXPixPerMeter;
    INTEGER        biYPixPerMeter;
    dword          biClrUsed;
    dword          biClrImporant;
} BITMAPINFOHEADER ;

//typedef struct tagBITMAPFILEHEADER 
//{
//  ushort             bfType;
//  dword          bfSize;
//  ushort             bfReserved1;
//  ushort             bfReserved2;
//  dword          bfOffBits;
//} BITMAPFILEHEADER ;
//
//typedef struct tagBITMAPCOREHEADER
//{
//    dword          bcSize;
//    short          bcWidth;
//    short          bcHeight;
//    ushort             bcPlanes;
//    ushort             bcBitCount;
//} BITMAPCOREHEADER ;
//
//typedef struct tagBITMAPINFOHEADER
//{
//    dword          biSize;
//    long           biWidth;
//    long           biHeight;
//    ushort             biPlanes;
//    ushort             biBitCount;
//    dword          biCompression;
//    dword          biSizeImage;
//    long           biXPixPerMeter;
//    long           biYPixPerMeter;
//    dword          biClrUsed;
//    dword          biClrImporant;
//} BITMAPINFOHEADER ;

#pragma pack( pop )	//アラインメント制御エンド

#endif

#include <iostream>
using namespace std;


/////////////////////////////////////////////////////////////////////////
//　　MMDPI_BMP class
/////////////////////////////////////////////////////////////////////////
class MMDPI_BMP
{
protected:
    GLuint bit_size;
    GLubyte *bits;
    GLenum format;
    GLuint internalFormat;
    GLuint width;
    GLuint height;
    GLuint bpp;

	GLuint	texture;

public:

	virtual GLuint	get_id( void );

	int				ReadBMP( const char *filename );
    virtual int		load( const char *filename );

	virtual int		draw( void );

    MMDPI_BMP();
    ~MMDPI_BMP();
};

