
#include "../mmdpi_include.h"

#ifndef		__MMDPI__DDS__DEFINES__
#define		__MMDPI__DDS__DEFINES__	( 1 )

const dword	MMDPI_DDPF_ALPHAPIXELS 	= 0x00000001; 	// RGB 以外に alpha が含まれている。つまり dwRGBAlphaBitMask が有効。
const dword	MMDPI_DDPF_ALPHA 	= 0x00000002; 	// pixel は Alpha 成分のみ含まれている。
const dword	MMDPI_DDPF_FOURCC 	= 0x00000004; 	// dwFourCC が有効。
const dword	MMDPI_DDPF_PALETTEINDEXED4 = 0x00000008; // * Palet 16 colors (DX9 以降はたぶん使用されない)
const dword	MMDPI_DDPF_PALETTEINDEXED8 = 0x00000020; // * Palet 256 colors (DX10 以降は使用されない)
const dword	MMDPI_DDPF_RGB 		= 0x00000040;	// dwRGBBitCount/dwRBitMask/dwGBitMask/dwBBitMask/dwRGBAlphaBitMask によってフォーマットが定義されていることを示す
const dword	MMDPI_DDPF_LUMINANCE 	= 0x00020000; 	// * 1ch のデータが R G B すべてに展開される
const dword	MMDPI_DDPF_BUMPDUDV 	= 0x00080000; 	// * pixel が符号付であることを示す (本来は bump 用) DX10以降は使用しない 

const dword	MMDPI_DDSCAPS2_CUBEMAP_POSITIVEX =	0x00000400; 	// Cubemap +X が含まれている
const dword	MMDPI_DDSCAPS2_CUBEMAP_NEGATIVEX =	0x00000800; 	// Cubemap -X が含まれている
const dword	MMDPI_DDSCAPS2_CUBEMAP_POSITIVEY =	0x00001000; 	// Cubemap +Y が含まれている
const dword	MMDPI_DDSCAPS2_CUBEMAP_NEGATIVEY =	0x00002000;	// Cubemap -Y が含まれている
const dword	MMDPI_DDSCAPS2_CUBEMAP_POSITIVEZ =	0x00004000; 	// Cubemap +Z が含まれている
const dword	MMDPI_DDSCAPS2_CUBEMAP_NEGATIVEZ =	0x00008000; 	// Cubemap -Z が含まれている 

const dword	MMDPI_DDS_FOURCC_DXT1	= 0x31545844;	// (MAKEFOURCC('D','X','T','1'))
const dword	MMDPI_DDS_FOURCC_DXT3	= 0x33545844;	// (MAKEFOURCC('D','X','T','3'))
const dword	MMDPI_DDS_FOURCC_DXT5	= 0x35545844;	// (MAKEFOURCC('D','X','T','5'))


#pragma pack( push, 1 )	//アラインメント制御をオフる

typedef struct tagMMDPI_DDS_PixelFormat
{
	dword			size;
	dword			flag;
	dword			fourCC;
	dword			bpp;
	dword			red_mask;
	dword			green_mask;
	dword			blue_mask;
	dword			alpha_mask;
} MMDPI_DDS_PixelFormat;

typedef struct tagMMDPI_DDS_HEADER
{
	char			magic[ 4 ];	// "DDS "

	dword			size;		// 常に 124
	dword			flag;

	dword			width;
	dword			height;
	dword			pitch;
	dword			depth;

	dword			mip_map_levels;
	dword			alpha_bit_depth;
	dword			reserved;
	dword			surface;

	dword			ck_dst_orverlay[ 2 ];
	dword			ck_dst_blt[ 2 ];
	dword			ck_src_orverlay[ 2 ];
	dword			ck_src_blt[ 2 ];

	MMDPI_DDS_PixelFormat	pixel_format;

	dword			caps0;
	dword			caps1;
	dword			caps2;
	dword			caps3;

	dword			texture_stage;

} MMDPI_DDS_HEADER;

#pragma pack( pop )	//アラインメント制御エンド

class MMDPI_DDS
{
protected:
	dword			size;
	dword			flag;

	dword			width;
	dword			height;
	dword			depth;

	MMDPI_DDS_HEADER	head;
	
	GLenum			format;
	GLuint			internal_format;
	GLuint			texture;
	GLubyte*		bits;
	
public:
	virtual GLuint		get_id( void );

	virtual uint		get_width( void );
	virtual uint		get_height( void );
	
	int			readFile( const char *filename );
	virtual int		load( const char *filename );

	MMDPI_DDS();
	~MMDPI_DDS();
};

#endif	//	__MMDPI__DDS__DEFINES__