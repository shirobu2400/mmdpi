
#include "../mmdpi_include.hpp"
#include "utf8_sjis.hpp"


#ifndef		__MMDPI__GETBIN__DEFINES__
#define		__MMDPI__GETBIN__DEFINES__	( 1 )

class GetBin
{

public:

	unsigned char	*buf, *buf_base;
	unsigned long	buf_len;

	char*		text_buf( char byte_size, uint* length = 0 );
	int		change_enmark( char* _string_ );
	unsigned char*	get_bin2( void* bin, int bin_size, int buf_size );
	unsigned char*	get_bin( void* bin, int size );
	int		load( const char *file_name );


	char		directory[ 0xff ];
	int		get_direcotory( const char *file_name );
	int		get_header( void );
	char*		bin_string( void );

	GetBin();
	~GetBin();
};

#endif	//	__MMDPI__GETBIN__DEFINES__
