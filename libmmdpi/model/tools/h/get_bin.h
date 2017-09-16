
#include "../../h/mmdpi_include.h"
#include "utf8tosjis.h"

typedef unsigned int uint;

#pragma once


class GetBin
{

public :

	unsigned char	*buf, *buf_base;
	//unsigned int	buf_len;
	unsigned long	buf_len;
	
	int				change_enmark( char* _string_ );
	unsigned char*	get_bin2( void* bin, int buf_size, int next_size );
	unsigned char*	get_bin( void* bin, int size );
	int				load( const char *file_name );

	
	char	directory[ 0xff ];
	int		get_direcotory( const char *file_name );
	int		get_header( void );
	char*	bin_string( void );

	char*	text_buf( char byte_size, uint* length = 0 );
	char*	convert_sjis( char* text, uint byte_len, int text_release_flag = 0 );
	char*	convert_utf8( char* text, uint byte_len );
	
	int		Utf16ToUtf8( char *dest, size_t dest_size, const short *src, size_t src_size );
	int		Utf8toSJIS( char *dest, size_t dest_size, const char *src, size_t src_size );
	char	utf8mbleb( char* src );

	int		char_byte( char* _string );


	GetBin();
	~GetBin();
} ;