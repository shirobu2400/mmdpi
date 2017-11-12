
#include "../mmdpi_include.h"

//#define _MMDPI_BITMAP_DRAW_SWITCH_

#ifndef		__MMDPI__BITMAP__DEFINES__
#define		__MMDPI__BITMAP__DEFINES__	( 1 )

#pragma pack( push, 1 )	//アラインメント制御をオフる

#if defined( _WINDOWS_ ) || defined( _WIN32 )
#	include <windows.h>
#else

typedef struct tagBITMAPFILEHEADER 
{
	ushort		bfType;
	dword		bfSize;
	ushort		bfReserved1;
	ushort		bfReserved2;
	dword		bfOffBits;
} BITMAPFILEHEADER ;

typedef struct tagBITMAPCOREHEADER
{
	dword		bcSize;
	SHORT		bcWidth;
	SHORT		bcHeight;
	ushort		bcPlanes;
	ushort		bcBitCount;
} BITMAPCOREHEADER ;

typedef struct tagBITMAPINFOHEADER
{
	dword		biSize;
	INTEGER		biWidth;
	INTEGER		biHeight;
	ushort		biPlanes;
	ushort		biBitCount;
	dword		biCompression;
	dword		biSizeImage;
	INTEGER		biXPixPerMeter;
	INTEGER		biYPixPerMeter;
	dword		biClrUsed;
	dword		biClrImporant;
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

#endif

#pragma pack( pop )	//アラインメント制御エンド

#include <iostream>
using namespace std;

class MMDPI_BMP
{
protected:
	GLuint		bit_size;
	GLubyte*	bits;
	GLenum		format;
	GLuint		internal_format;
	GLuint		width;
	GLuint		height;
	GLuint		bpp;

	GLuint		texture;
	GLubyte		bitc;
	
public:

	virtual GLuint		get_id( void );

	int			ReadBMP( const char *filename );
	virtual int		load( const char *filename );

	virtual int		draw( void );

    MMDPI_BMP();
    ~MMDPI_BMP();
};

#endif	//	__MMDPI__BITMAP__DEFINES__