
#include "get_bin.h"


int	GetBin::change_enmark( char* _string_ )
{
	int		c = 0;
	while( *_string_ )
	{
		if( *_string_ == '\\' )
			*_string_ = '/';
		_string_ ++;
	}
	return c;
}

// get binary code from buf
unsigned char* GetBin::get_bin( void *bin, int size )
{
	if( ( unsigned int )( buf - buf_base + ( unsigned int )size ) > ( unsigned int )buf_len )
	{
		memset( bin, 0, size );
		return NULL;
	}
	memcpy( bin, ( void* )buf, size );
	buf += size;
	return buf;
}

unsigned char* GetBin::get_bin2( void* bin, int buf_size, int next_size )
{
	if( ( unsigned int )( buf - buf_base + ( unsigned int )buf_size ) > ( unsigned int )buf_len )
		return NULL;
	
	memset( bin, 0, buf_size );
	memcpy( bin, ( void* )buf, next_size );
	
	buf += next_size;
	return buf;
}

int	GetBin::load( const char *file_name )
{
	FILE*	fp = fopen( file_name, "rb" );
	if( fp == NULL )
		return -1;

	get_direcotory( file_name );

	// Get File Size
	fseek( fp, 0l, SEEK_END );
	//if( fgetpos( fp, &buf_len ) )
	//	return -1;
	buf_len = ( unsigned long )ftell( fp );
	fseek( fp, 0l, SEEK_SET );

	buf_base = buf = new unsigned char[ ( unsigned int )buf_len + 2 ];
	if( buf == NULL )
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
	char*	text	 = NULL;
	long	byte_len = 0;

	if( length )
		*length = 0;

	get_bin( &byte_len, 4 );
	if( byte_len < 1 )
		return NULL;
	
	text = new char[ byte_len + 4 ];
	if( text == NULL )
	{
		puts( "Text buf cannot Allocation." );
		return NULL;
	}
	memset( text, 0, byte_len + 4 );
	get_bin( text, byte_len );
	text[ byte_len ] = '\0';
		
	if( byte_size > 1 )
	{
		text[ byte_len + 1 ] = '\0';
		text = convert_utf8( text, byte_len / byte_size + 1 );
		byte_len = strlen( text );
	}

	change_enmark( text );

	if( length )
		*length = byte_len;
	//
	return text;
}

char* GetBin::convert_sjis( char* text, uint byte_len, int text_release_flag )
{
	if( byte_len < 1 )
		return NULL;

	char*		text2 = new char[ byte_len + 4 ];
	memset( text2, 0, byte_len + 2 );
	Utf8toSJIS( text2, byte_len, ( const char * )text, byte_len );

	if( text_release_flag )
		delete[] text;

	return text2;
}

int	GetBin::Utf8toSJIS( char *dest, size_t dest_size, const char *src, size_t src_size )
{
    char				mb[ 2 ] = { 0 };  
	unsigned short		wc = 0;
	
    for( uint i = 0, j = 0; src[ j ] && j < src_size; i ++, j ++ )
	{
		unsigned char	c = ( unsigned char )src[ j ] & 0xff;
		wc = 0x00;
		if( c < 0x80 )
            wc = c & 0xff;
        else if( c < 0xc0 )
            continue;
        else if( c < 0xe0 )
		{
            wc = ( unsigned short )( c & 0x1f ) << 6;
			j ++;
			if( ( c = src[ j ] ) == 0x00 ) 
				break;
            wc |= c & 0x3f;
        }
        else if( c < 0xf0 )
		{
            wc = ( unsigned short )( src[ j ] & 0x0f ) << 12;
			j ++;
            if( ( c = src[ j ] ) == 0x00 )
				break;
			wc |= ( unsigned short )( c & 0x3f ) << 6;
            j ++;
			if( ( c = src[ j ] ) == 0x00 )
				break;
			wc |= c & 0x3f;
        }
        else
            continue;

		//c = wctomb( mb, wc );
		mb[ 1 ] = ( char )( utf8tosjis[ wc ] & 0xff );
		mb[ 0 ] = ( char )( ( utf8tosjis[ wc ] >> 8 ) & 0xff );

		c = ( ( mb[ 0 ] )? 1 : 0 ) + ( ( mb[ 1 ] )? 1 : 0 );
		if( c == 1 ) 
		{
			dest[ i ] = mb[ 1 ];
		}
		else if( c > 1 )
		{
			dest[ i ] = mb[ 0 ];
			i ++;
			dest[ i ] = mb[ 1 ];
		}
    }

	return 0;
}

/**
 * 文字コードをUTF-16よりUTF-8へと変換。
 * 
 * @param[out] dest 出力文字列UTF-8
 * @param[in]  dest_size destのバイト数
 * @param[in]  src 入力文字列UTF-16
 * @param[in]  src_size 入力文字列の文字数
 * 
 * @return 成功時には出力文字列のバイト数を戻します。
 *         dest_size に0を指定し、こちらの関数を呼び出すと、変換された
 *         文字列を格納するのに必要なdestのバイト数を戻します。
 *         関数が失敗した場合には、FALSEを戻します。
 */
int GetBin::Utf16ToUtf8( char *dest, size_t dest_size, const short *src, size_t src_size )
{
/* ビットパターン
 * ┌───────┬───────┬───────────────────────────┬──────────────────┐
 * │フォーマット  │Unicode       │UTF-8ビット列                                         │Unicodeビット列                     │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │1バイトコード │\u0～\u7f     │0aaabbbb                                              │00000000 0aaabbbb                   │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │2バイトコード │\u80～\u7ff   │110aaabb 10bbcccc                                     │00000aaa bbbbcccc                   │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │3バイトコード │\u800～\uffff │1110aaaa 10bbbbcc 10ccdddd                            │aaaabbbb ccccdddd                   │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │4バイトコード │--------------│11110??? 10?????? 10?????? 10??????                   │未対応                              │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │5バイトコード │--------------│111110?? 10?????? 10?????? 10?????? 10??????          │未対応                              │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │6バイトコード │--------------│1111110? 10?????? 10?????? 10?????? 10?????? 10?????? │未対応                              │
 * └───────┴───────┴───────────────────────────┴──────────────────┘
 */
    long				countNeedsBytes;
    unsigned long		cursor;
    unsigned short		wcWork1;
    long				sizeBytes;
    char				chWork1;
    char				chWork2;
    char				chWork3;
    
    if( src == 0x00 )
    {
        /* Error : src is NULL. */
        return -1;
    }
    if( src_size < 0 )
    {
        /* Error : src_size < 0. */
        return -1;
    }
    
    countNeedsBytes = 0;
    for( cursor = 0; cursor < src_size; cursor ++ )
    {
        /* srcより1ワードのデータを読み出し */
       if( *( unsigned char* )src + cursor == ( unsigned char )0x00 )
			break;

        wcWork1 = ( unsigned short )*( ( const unsigned short* )src + cursor );
		if( wcWork1 <= ( unsigned short )0x007f )
        {
            /* 0x0000 to 0x007f */
            sizeBytes = 1;
        }
        else if( ( unsigned short )0x0080 <= wcWork1 && wcWork1 <= ( unsigned short )0x07ff )
        {
            /* 0x0080 to 0x07ff */
            sizeBytes = 2;
        }
        else if( ( unsigned short )0x0800 <= wcWork1 )
        {
            /* 0x0800 to 0xffff */
            sizeBytes = 3;
        }
        else
        {
            /* Error : unknown code */
            return -1;
        }

        /* sizeBytes毎に処理を分岐 */
        if( 0 < dest_size && dest )
		{
			/*
			 * dest_size をチェック
			 */
			if( dest_size < ( unsigned )( countNeedsBytes + sizeBytes ) )
			{
				/* Error : memory is not enough for dest */
				return countNeedsBytes;
			}
        
			switch( sizeBytes )
			{
			case 1:
				/*
				 * ビット列
				 * (0aaabbbb)UTF-8 ... (00000000 0aaabbbb)UTF-16
				 */

				*dest = ( char )wcWork1;              /* 0aaabbbb */
				dest ++;
				break;
			case 2:
				/*
				 * ビット列
				 * (110aaabb 10bbcccc)UTF-8 ... (00000aaa bbbbcccc)UTF-16
				 */
				chWork1 =  ( char )( wcWork1 >> 6 );     /* 000aaabb */
				chWork1 |= ( char )0xc0;              /* 110aaabb */
				chWork2 =  ( char )wcWork1 & 0xff;            /* bbbbcccc */
				chWork2 &= ( char )0x3f;              /* 00bbcccc */
				chWork2 |= ( char )0x80;              /* 10bbcccc */

				*dest = chWork1;
				dest++;
				*dest = chWork2;
				dest++;
				break;
			case 3:
				/*
				 * ビット列
				 * (1110aaaa 10bbbbcc 10ccdddd)UTF-8 ... (aaaabbbb ccccdddd)UTF-16
				 */
				chWork1 =  ( char )( wcWork1 >> 12 );    /* ????aaaa */
				chWork1 &= ( char )0x0f;              /* 0000aaaa */
				chWork1 |= ( char )0xe0;              /* 1110aaaa */
				chWork2 =  ( char )( wcWork1 >> 6 );     /* aabbbbcc */
				chWork2 &= ( char )0x3f;              /* 00bbbbcc */
				chWork2 |= ( char )0x80;              /* 10bbbbcc */
				chWork3 =  ( char )wcWork1 & 0xff;            /* ccccdddd */
				chWork3 &= ( char )0x3f;              /* 00ccdddd */
				chWork3 |= ( char )0x80;              /* 10ccdddd */

				*dest = chWork1;
				dest++;
				*dest = chWork2;
				dest++;
				*dest = chWork3;
				dest++;
				break;
			default:
				break;
			}
		}

        countNeedsBytes += sizeBytes;
    }

    return countNeedsBytes;
}

char GetBin::utf8mbleb( char* src )
{
    /* srcより1ワードのデータを読み出し */
	unsigned short	wcWork1 = ( unsigned short )*( ( const unsigned short* )src );
	char			sizeBytes = 1;

	//if( wcWork1 == ( unsigned short )0x0000 )
	//	break;

    if( ( wcWork1 <= ( ( unsigned short )0x007f ) ) )
    {
        /* 0x0000 to 0x007f */
        sizeBytes = 1;
    }
    else if( ( ( ( unsigned short )0x0080 ) <= wcWork1 ) && ( wcWork1 <= ( ( unsigned short )0x07ff ) ) )
    {
        /* 0x0080 to 0x07ff */
        sizeBytes = 2;
    }
    else if ( ( ( ( unsigned short )0x0800 ) <= wcWork1 ) )
    {
        /* 0x0800 to 0xffff */
        sizeBytes = 3;
    }
    else
    {
        /* Error : unknown code */
        return 1;
    }

	return sizeBytes;
}

char* GetBin::convert_utf8( char* text, uint byte_len )
{
	int		length = ( int )Utf16ToUtf8( NULL, 0, ( const short* )text, byte_len );
	char*	text2 = new char[ length + 2 ];
	if( text2 == NULL )
	{
		printf( "Text buf cannot Allocation. : %d length.\n", length );
		return NULL;
	}

	Utf16ToUtf8( text2, length, ( const short* )text, byte_len );
	delete[] text;

	return text2;
}
	//バイナリ用文字列抜き出し
char* GetBin::bin_string( void )
{
	char*			result;
	vector<char>	_string;
	char			c[ 8 ];

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
	buf			= 0x00;
	buf_len		= 0x00;
}

GetBin::~GetBin()
{
	delete[]	buf_base;
}
