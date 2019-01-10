#include "../mmdpi_include.h"


#ifndef		__MMDPI__UTF8SJIS__DEFINES__
#define		__MMDPI__UTF8SJIS__DEFINES__	( 1 )

typedef struct tagCharByte
{
	unsigned int	len;
	unsigned int	byte;
} CharByte;

int		cc_char_sjis_to_utf8( char* utf8, int* utf8_step, const char* sjis, int* sjis_step );
int		cc_char_utf8_to_sjis( char* sjis, int* sjis_step, const char* utf8, int* utf8_step );
int		cc_create_tables( void );


int		cconv_utf8_to_sjis( char* sjis, const char* utf8 );
int		cconv_sjis_to_utf8( char* utf8, const char* sjis );
int		cconv_utf16_to_utf8( char *utf8, const short *utf16 );

#endif	//	__MMDPI__UTF8SJIS__DEFINES__