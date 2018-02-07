
#include "get_bin.h"


int GetBin::change_enmark( char* _string_ )
{
	int		c = 0;
	while( *_string_ )
	{
		if( *_string_ == '\\' )
		{
			*_string_ = '/';
			c ++;
		}
		_string_ ++;
	}
	return c;
}

// get binary code from buf
unsigned char* GetBin::get_bin( void *bin, int size )
{
	if( ( unsigned int )( buf - buf_base + ( unsigned int )size ) > ( unsigned int )buf_len )
	{
		memset( bin, 0x00, size );
		return 0x00;
	}
	memcpy( bin, ( const void* )buf, size );
	buf += size;
	return buf;
}

unsigned char* GetBin::get_bin2( void* bin, int bin_size, int buf_size )
{
	unsigned int	bin_i = ( intptr_t )bin;

	if( ( unsigned int )( buf - buf_base + ( unsigned int )bin_size ) > ( unsigned int )buf_len )
	{
		memset( bin, 0, bin_size );
		return 0x00;
	}

	memset( ( void* )bin_i, 0x00, bin_size );
	memcpy( ( void* )bin_i, ( void* )buf, buf_size );

	buf += buf_size;
	return buf;
}

int GetBin::load( const char *file_name )
{
	FILE*	fp = fopen( file_name, "rb" );
	if( fp == 0x00 )
		return -1;

	get_direcotory( file_name );

	// Get File Size
	fseek( fp, 0l, SEEK_END );
	//if( fgetpos( fp, &buf_len ) )
	//	return -1;
	buf_len = ( unsigned long )ftell( fp );
	fseek( fp, 0l, SEEK_SET );

	buf_base = buf = new unsigned char[ ( unsigned int )buf_len + 2 ];
	if( buf == 0x00 )
		return -1;

	size_t	get_size;
	get_size = fread( buf, sizeof( unsigned char ), ( unsigned int )buf_len, fp );

	fclose( fp );

	if( get_size != buf_len )
		return -1;
	return 0;
}

char* GetBin::text_buf( char byte_size, uint* length )
{
	char*	text1	 = 0x00;
	char*	text2	 = 0x00;
	long	byte_len = 0;

	if( length )
		*length = 0;

	get_bin( &byte_len, 4 );
	if( byte_len < 1 )
		return 0x00;

	text1 = new char[ byte_len + 4 ];
	if( text1 == 0x00 )
	{
		puts( "Text buf cannot Allocation." );
		return 0x00;
	}
	memset( text1, 0, byte_len + 4 );
	get_bin( text1, byte_len );
	text1[ byte_len ] = '\0';

	if( byte_size > 1 )
	{
		text1[ byte_len + 1 ] = '\0';
		text2 = new char[ cconv_utf16_to_utf8( 0x00, ( const short* )text1 ) * 2 + 4 ];
		byte_len = cconv_utf16_to_utf8( text2, ( const short* )text1 );
		delete[] text1;
		text1 = text2;
	}

	change_enmark( text1 );

	if( length )
		*length = byte_len;
	//
	return text1;
}

//バイナリ用文字列抜き出し
char* GetBin::bin_string( void )
{
	char*		result;
	vector<char>	_string;
	char		c[ 8 ];

	while( get_bin( c, 1 ) && c[ 0 ] )
		_string.push_back( c[ 0 ] );
	_string.push_back( '\0' );
	result = new char[ _string.size() + 1 ];
	for( unsigned int i = 0; i < _string.size(); i ++ )
		result[ i ] = _string[ i ];

	return result;
}

int GetBin::get_direcotory( const char *file_name )
{
	int		i;
	int		len = strlen( file_name );
	int		d_line = len - 1;

	directory[ 0 ] = '\0';

#ifndef _WIN32
	setlocale( LC_CTYPE, "ja_JP.UTF-8" );
#else
	setlocale( LC_CTYPE, "jpn" );
#endif
	for( i = 0; i < len; i += 1/*this->utf8mbleb( &file_name[ i ] )*/ )
	{
		if( file_name[ i ] == '/' || file_name[ i ] == '\\' )
			d_line = i;
	}

	if( d_line > 0 )
	{
		strncpy( directory, file_name, d_line );
		directory[ d_line ++ ] = '/';
		directory[ d_line ] = '\0';
	}

	return 0;
}


GetBin::GetBin()
{
	buf_base	= 0;
	buf		= 0x00;
	buf_len		= 0x00;
}

GetBin::~GetBin()
{
	delete[] buf_base;
}
