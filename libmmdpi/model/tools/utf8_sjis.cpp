#include "utf8_sjis.hpp"
#include "raw_table.hpp"


static std::map<unsigned int, CharByte>	cc_sjis_to_utf8;
static std::map<unsigned int, CharByte>	cc_utf8_to_sjis;


int cc_char_sjis_to_utf8( char* utf8, int* utf8_step, const char* sjis, int* sjis_step )
{
	unsigned int		sjisi;
	CharByte		utf8_cb;
	int			i;

	if( sjis == 0x00 )
		return -1;
	sjisi = ( unsigned char )0;

	if( sjis_step )
		*sjis_step = 0;
	if( utf8_step )
		*utf8_step = 0;

	sjisi = 0x00;
	sjisi |= ( ( unsigned char )*( sjis + 0 ) ) << 16;
	sjisi |= ( ( unsigned char )*( sjis + 1 ) ) <<  8;
	sjisi |= ( ( unsigned char )*( sjis + 2 ) );
	i = 3;
	while( sjisi && cc_sjis_to_utf8.count( sjisi ) == 0 )
	{
		sjisi >>= 8;
		i --;
	}
	if( sjisi == 0x00 )
		return 0;

	utf8_cb = cc_sjis_to_utf8[ sjisi ];
	if( sjis_step )
		*sjis_step = i;
	if( utf8_step )
		*utf8_step = utf8_cb.len;

	if( utf8 == 0x00 )
		return 0;
	for( i = ( signed )utf8_cb.len - 1; i >= 0; i -- )
	{
		*utf8 = ( char )( ( utf8_cb.byte >> ( i * 8 ) ) & 0xff );
		utf8 ++;
	}

	return 0;
}

int cc_char_utf8_to_sjis( char* sjis, int* sjis_step, const char* utf8, int* utf8_step )
{
	unsigned int		utf8i;
	CharByte		sjis_cb;
	int			i;


	if( utf8 == 0x00 )
		return -1;

	if( sjis_step )
		*sjis_step = 0;
	if( utf8_step )
		*utf8_step = 0;

	utf8i = 0x00;
	utf8i |= ( ( unsigned char )*( utf8 + 0 ) ) << 16;
	utf8i |= ( ( unsigned char )*( utf8 + 1 ) ) <<  8;
	utf8i |= ( ( unsigned char )*( utf8 + 2 ) );
	i = 3;
	while( utf8i && cc_utf8_to_sjis.count( utf8i ) == 0 )
	{
		utf8i >>= 8;
		i --;
	}
	if( utf8i == 0x00 )
		return 0;

	sjis_cb = cc_utf8_to_sjis[ utf8i ];
	if( sjis_step )
		*sjis_step = sjis_cb.len;
	if( utf8_step )
		*utf8_step = i;

	if( sjis == 0x00 )
		return 0;
	for( i = ( signed )sjis_cb.len - 1; i >= 0; i -- )
	{
		*sjis = ( char )( ( sjis_cb.byte >> ( i * 8 ) ) & 0xff );
		sjis ++;
	}

	return 0;
}

int cc_create_tables( void )
{
	for( int i = 0; raw_table[ i ][ 0 ] || raw_table[ i ][ 1 ]; i ++ )
	{
		unsigned int	utf8 = raw_table[ i ][ 0 ];
		unsigned int	sjis = raw_table[ i ][ 1 ];
		CharByte	utf8_cb;
		CharByte	sjis_cb;

		utf8_cb.byte = utf8;
		utf8_cb.len = 0;
		utf8_cb.len = ( utf8 & 0x0000ff )? 1 : utf8_cb.len ;
		utf8_cb.len = ( utf8 & 0x00ff00 )? 2 : utf8_cb.len ;
		utf8_cb.len = ( utf8 & 0xff0000 )? 3 : utf8_cb.len ;

		sjis_cb.byte = sjis;
		sjis_cb.len = 0;
		sjis_cb.len = ( sjis & 0x0000ff )? 1 : sjis_cb.len ;
		sjis_cb.len = ( sjis & 0x00ff00 )? 2 : sjis_cb.len ;
		sjis_cb.len = ( sjis & 0xff0000 )? 3 : sjis_cb.len ;

		cc_utf8_to_sjis[ utf8 ] = sjis_cb;
		cc_sjis_to_utf8[ sjis ] = utf8_cb;
	}

	return 0;
}

int cconv_sjis_to_utf8( char* utf8, const char* sjis )
{
	int	step_utf8 = 1;
	int	step_sjis = 1;
	int	j = 0, k;

	if( utf8 == 0x00 )
	{
		for( j = 0, k = 0; step_utf8 && step_sjis; j += step_utf8, k += step_sjis )
			cc_char_sjis_to_utf8( 0x00, &step_utf8, sjis + k, &step_sjis );
		return j;
	}
	for( j = 0, k = 0; step_utf8 && step_sjis; j += step_utf8, k += step_sjis )
		cc_char_sjis_to_utf8( utf8 + j, &step_utf8, sjis + k, &step_sjis );
	*( utf8 + j ) = '\0';
	return j;
}

int cconv_utf8_to_sjis( char* sjis, const char* utf8 )
{
	int	step_utf8 = 1;
	int	step_sjis = 1;
	int	j = 0, k;

	if( sjis == 0x00 )
	{
		for( j = 0, k = 0; step_utf8 && step_sjis && *( utf8 + k ); j += step_sjis, k += step_utf8 )
			cc_char_utf8_to_sjis( 0x00, &step_sjis, utf8 + k, &step_utf8 );
		return j;
	}
	for( j = 0, k = 0; step_utf8 && step_sjis && *( utf8 + k ); j += step_sjis, k += step_utf8 )
		cc_char_utf8_to_sjis( sjis + j, &step_sjis, utf8 + k, &step_utf8 );
	*( sjis + j ) = '\0';
	return j;
}

int cconv_utf16_to_utf8( char *utf8, const short *utf16 )
{
	int			j;
	int			i;
	unsigned short		a_word;
	long			byte_size;
	char			ch1;
	char			ch2;
	char			ch3;


	if( utf16 == 0x00 )
		return -1;	// error

	j = 0;
	for( i = 0; utf16[ i ]; i ++ )
	{
		/* src???1???[?h??f?[?^????o?? */
		if( *( unsigned char* )utf16 + i == ( unsigned char )0x00 )
			break;

		a_word = ( unsigned short )*( ( const unsigned short* )utf16 + i );
		if( a_word <= ( unsigned short )0x007f )
		{
			/* 0x0000 to 0x007f */
			byte_size = 1;
		}
		else if( ( unsigned short )0x0080 <= a_word && a_word <= ( unsigned short )0x07ff )
		{
			/* 0x0080 to 0x07ff */
			byte_size = 2;
		}
		else if( ( unsigned short )0x0800 <= a_word )
		{
			/* 0x0800 to 0xffff */
			byte_size = 3;
		}
		else
		{
			/* Error : unknown code */
			return -1;
		}


		j += byte_size;

		/* sizeBytes?????????? */
		if( utf8 == 0x00 )
			continue;

		switch( byte_size )
		{
		case 1:
			/*
			* ?r?b?g??
			* (0aaabbbb)UTF-8 ... (00000000 0aaabbbb)UTF-16
			*/

			*utf8 = ( char )a_word;              /* 0aaabbbb */
			utf8 ++;
			break;
		case 2:
			/*
			* ?r?b?g??
			* (110aaabb 10bbcccc)UTF-8 ... (00000aaa bbbbcccc)UTF-16
			*/
			ch1 =  ( char )( a_word >> 6 );	/* 000aaabb */
			ch1 |= ( char )0xc0;		/* 110aaabb */
			ch2 =  ( char )a_word & 0xff;	/* bbbbcccc */
			ch2 &= ( char )0x3f;		/* 00bbcccc */
			ch2 |= ( char )0x80;		/* 10bbcccc */

			*utf8 = ch1;
			utf8++;
			*utf8 = ch2;
			utf8++;
			break;
		case 3:
			/*
			* ?r?b?g??
			* (1110aaaa 10bbbbcc 10ccdddd)UTF-8 ... (aaaabbbb ccccdddd)UTF-16
			*/
			ch1 =  ( char )( a_word >> 12 );	/* ????aaaa */
			ch1 &= ( char )0x0f;			/* 0000aaaa */
			ch1 |= ( char )0xe0;			/* 1110aaaa */
			ch2 =  ( char )( a_word >> 6 );		/* aabbbbcc */
			ch2 &= ( char )0x3f;			/* 00bbbbcc */
			ch2 |= ( char )0x80;			/* 10bbbbcc */
			ch3 =  ( char )a_word & 0xff;		/* ccccdddd */
			ch3 &= ( char )0x3f;			/* 00ccdddd */
			ch3 |= ( char )0x80;			/* 10ccdddd */

			*utf8 = ch1;
			utf8++;
			*utf8 = ch2;
			utf8++;
			*utf8 = ch3;
			utf8++;
			break;
		}

		*utf8 = '\0';
	}

	return j;
}
