
#include "../mmdpi_include.h"
#include "utf8tosjis.h"
#include "raw_table.h"


typedef unsigned int uint;

#pragma once


typedef struct tagCharByte
{
	unsigned int	len;
	unsigned int	byte;
} CharByte;


static map<unsigned int, CharByte>	cc_sjis_to_utf8;
static map<unsigned int, CharByte>	cc_utf8_to_sjis;
int					cc_char_sjis_to_utf8( char* utf8, int* utf8_step, const char* sjis, int* sjis_step );
int					cc_char_utf8_to_sjis( char* sjis, int* sjis_step, const char* utf8, int* utf8_step );
int					cc_create_tables( void );

class CharCode
{
public:
	static char*	convert_sjis( char* text, uint byte_len, int text_release_flag = 0 );
	static char*	convert_utf8( char* text, uint byte_len, int text_release_flag = 0 );
	
	static int	Utf16ToUtf8( char *dest, size_t dest_size, const short *src, size_t src_size );
	static int	Utf8toSJIS( char *dest, size_t dest_size, const char *src, size_t src_size );
	static char	utf8mbleb( char* src );

	static int	char_byte( char* _string );
} ;

class GetBin: public CharCode
{

public :

	unsigned char	*buf, *buf_base;
	//unsigned int	buf_len;
	unsigned long	buf_len;
	
	char*		text_buf( char byte_size, uint* length = 0 );
	int		change_enmark( char* _string_ );
	unsigned char*	get_bin2( void* bin, int buf_size, int next_size );
	unsigned char*	get_bin( void* bin, int size );
	int		load( const char *file_name );

	
	char		directory[ 0xff ];
	int		get_direcotory( const char *file_name );
	int		get_header( void );
	char*		bin_string( void );

	GetBin();
	~GetBin();
} ;
