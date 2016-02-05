#pragma once

#include "mmdpi_define.h"

#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdio>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <clocale>

#ifdef _MMDPI_OPENGL_ES_DEFINES_	// OpenGL ES 2.0
	#include <GLES2/gl2.h>
	#include <EGL/egl.h>
#else
	#include "GL/glew.h"
	#include <GL/gl.h>
#endif

#include <vector>
#include <map>
#include <string>
using namespace std;

#ifndef _MMDPI_AMD64BIT_
	//	32bit
	typedef long			integer;
	typedef unsigned long	dword;
#else
	//	64bit amd
	typedef int				integer;
	typedef unsigned int	dword;
#endif

typedef dword dword;
typedef integer INTEGER;
typedef unsigned char byte, BYTE;
typedef unsigned short ushort, ushort;
typedef short SHORT;

typedef unsigned int uint;

//	テクスチャ
typedef GLuint				mmdpiTexture;

//#pragma pack( push, 1 )	//アラインメント制御をオフる

//	カラー
typedef struct _mmdpiColor_
{
	float	r;
	float	g;
	float	b;
	float	a;

	_mmdpiColor_* ptr( void )
	{
		return this;
	}

	_mmdpiColor_()
	{
		r = g = b = a = 0;
	}
} mmdpiColor, *mmdpiColor_ptr;

// Matrial
typedef struct _mmdpiMaterial_
{
	mmdpiColor		ambient;
	mmdpiColor		diffuse;
	mmdpiColor		speculre;
} mmdpiMaterial, *mmdpiMaterial_ptr;

//	2D Vector
typedef struct _mmdpiVector2d_
{
	float			x, y;
		
	inline float& operator[]( int i )
	{
		return ( ( float* )this )[ i ];
	}

	inline _mmdpiVector2d_ operator=( const _mmdpiVector2d_& v1 )
	{
		x = v1.x;
		y = v1.y;
		return *this;
	}
	inline _mmdpiVector2d_ operator+( const _mmdpiVector2d_& v1 )
	{
		_mmdpiVector2d_		v;
		v.x = x + v1.x; 
		v.y = y + v1.y;
		return v;
	}
	inline _mmdpiVector2d_ operator-( const _mmdpiVector2d_& v1 )
	{
		_mmdpiVector2d_		v;
		v.x = x - v1.x;
		v.y = y - v1.y;
		return v;
	}
	inline _mmdpiVector2d_ operator*( float s )
	{
		_mmdpiVector2d_		v;
		v.x = x * s; 
		v.y = y * s;
		return v;
	}

	_mmdpiVector2d_( const _mmdpiVector2d_& v )
	{
		x = v.x;
		y = v.y;
	}

	_mmdpiVector2d_()
	{
		x = 0; 
		y = 0;
	}

} mmdpiVector2d, mmdpiVector2d_ptr;

typedef struct _mmdpiVector3d_
{
	float	x, y, z;
	
	inline float& operator[]( int i )
	{
		return ( ( float* )this )[ i ];
	}
	
	inline struct _mmdpiVector3d_ operator=( const _mmdpiVector3d_& v1 )
	{
		x = v1.x; 
		y = v1.y; 
		z = v1.z;
		return *this;
	}
	inline _mmdpiVector3d_ operator+( const _mmdpiVector3d_& v1 )
	{
		_mmdpiVector3d_		v;
		v.x = x + v1.x;
		v.y = y + v1.y; 
		v.z = z + v1.z;
		return v;
	}
	inline _mmdpiVector3d_ operator-( const _mmdpiVector3d_& v1 )
	{
		_mmdpiVector3d_		v;
		v.x = x - v1.x; 
		v.y = y - v1.y; 
		v.z = z - v1.z;
		return v;
	}
	inline _mmdpiVector3d_ operator*( float s )
	{
		_mmdpiVector3d_		v;
		v.x = x * s;
		v.y = y * s;
		v.z = z * s;
		return v;
	}

	inline _mmdpiVector3d_ normalize( void )
	{
		float	d = 0;

		d += x * x;
		d += y * y;
		d += z * z;
		d = sqrt( d );
		if( -1e-8f < d && d < 1e-8f )
			return *this;
		d = 1.0f / d;
		x = x * d;
		y = y * d;
		z = z * d;

		return *this;
	}

	inline float dot( const _mmdpiVector3d_& v )
	{
		float	d = 0;
		d += x * v.x;
		d += y * v.y;
		d += z * v.z;
		return d;
	}

	inline _mmdpiVector3d_ cross( const _mmdpiVector3d_& v )
	{
		_mmdpiVector3d_	c;
		c.x = y * v.z - z * v.y;
		c.y = z * v.x - x * v.z;
		c.z = x * v.y - y * v.x;
		return c;
	}

	inline float length_sq( const _mmdpiVector3d_& v )
	{
		float	d = 0, n;
		n = x - v.x;
		d += n * n;
		n = y - v.y;
		d += n * n;
		n = z - v.z;
		d += n * n;
		return d;
	}
  
	_mmdpiVector3d_()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	_mmdpiVector3d_( const _mmdpiVector3d_& v )
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	_mmdpiVector3d_( float x, float y, float z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

} mmdpiVector3d, *mmdpiVector3d_ptr;


// 4D Vector
typedef struct	_mmdpiVector4d_
{
	float		x, y, z, w;
	inline float& operator[]( int i )
	{
		return ( ( float* )this )[ i ];
	}

	inline float at( int i )
	{
		return operator[]( i );
	}
	
	inline struct _mmdpiVector4d_ operator=( const _mmdpiVector4d_& v1 )
	{
		x = v1.x; 
		y = v1.y; 
		z = v1.z; 
		w = v1.w;
		return *this;
	}
	inline _mmdpiVector4d_ operator+( const _mmdpiVector4d_& v1 )
	{
		_mmdpiVector4d_		v;
		v.x = x + v1.x; 
		v.y = y + v1.y; 
		v.z = z + v1.z;
		v.w = w + v1.w;
		return v;
	}
	inline _mmdpiVector4d_ operator-( const _mmdpiVector4d_& v1 )
	{
		_mmdpiVector4d_		v;
		v.x = x - v1.x;
		v.y = y - v1.y;
		v.z = z - v1.z;
		v.w = w - v1.w;
		return v;
	}
	inline _mmdpiVector4d_ operator*( float s )
	{
		_mmdpiVector4d_		v;
		v.x = x * s; 
		v.y = y * s; 
		v.z = z * s;
		v.w = w * s;
		return v;
	}

	inline _mmdpiVector4d_ normalize( void )
	{
		float	d = 0;

		d += x * x;
		d += y * y;
		d += z * z;
		d += w * w;
		d = sqrt( d );
		if( -1e-8f < d && d < 1e-8f )
			return *this;
		d = 1.0f / d;
		x = x * d;
		y = y * d;
		z = z * d;
		w = w * d;

		return *this;
	}

	inline float dot( const _mmdpiVector4d_& v )
	{
		float	d = 0;
		d += x * v.x;
		d += y * v.y;
		d += z * v.z;
		d += w * v.w;
		return d;
	}

	inline float length_sq( const _mmdpiVector4d_& v )
	{
		float	d = 0, n;
		n = x - v.x;
		d += n * n;
		n = y - v.y;
		d += n * n;
		n = z - v.z;
		d += n * n;
		n = w - v.w;
		d += n * n;
		return d;
	}
  
  
	////////////////////////////////////////////
	//	線形球面補間
	//
	inline struct _mmdpiVector4d_ slerp_quaternion(
		const _mmdpiVector4d_& q1,
		const _mmdpiVector4d_& q2,
		float time_f
	)
	{
		float	qr	= q1.x * q2.x 
					+ q1.y * q2.y
					+ q1.z * q2.z
					+ q1.w * q2.w;
		float	ss = 1.0f - ( qr * qr );
			
		*this = q1;

		if( ss < 0.0f )
			return *this;

		float	ph; 
		float	sin_ph;
		float	s1, s2;

		if( 1.0f < qr )
		{
			*this = q2;
			return *this;
		}
		ph = acos( qr ); 
		sin_ph = sin( ph );
		if( sin_ph == 0.0f )
			return *this;

		if( qr < 0.0f && 1.57079f < ph )
		{	
			qr = - q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
			if( 1.0f < qr )
			{
				*this = q2;
				return *this;
			}
			ph = acos( qr );
			sin_ph = sin( ph );
			if( sin_ph == 0.0f )
				return *this;
			s1 = sin( ph * ( 1.0f - time_f ) ) / sin_ph;
			s2 = sin( ph * time_f			 ) / sin_ph;

			this->x = q1.x * s1 - q2.x * s2;
			this->y = q1.y * s1 - q2.y * s2;
			this->z = q1.z * s1 - q2.z * s2;
			this->w = q1.w * s1 - q2.w * s2;

			return *this;
		}

		s1 = sin( ph * ( 1.0f - time_f ) ) / sin_ph;
		s2 = sin( ph * time_f			 ) / sin_ph;

		this->x = q1.x * s1 + q2.x * s2;
		this->y = q1.y * s1 + q2.y * s2;
		this->z = q1.z * s1 + q2.z * s2;
		this->w = q1.w * s1 + q2.w * s2;

		return *this;
	}

	_mmdpiVector4d_( const _mmdpiVector4d_& c )
	{
		this->x = c.x;
		this->y = c.y;
		this->z = c.z;
		this->w = c.w;
	}

	_mmdpiVector4d_()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}

	_mmdpiVector4d_( float x, float y, float z, float w )
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

} mmdpiVector4d, *mmdpiVector4d_ptr, mmdpiQuaternion, *mmdpiQuaternion_ptr;


// Matrix
typedef struct _mmdpiMatrix_
{
	float		_11, _21, _31, _41; 
	float		_12, _22, _32, _42; 
	float		_13, _23, _33, _43; 
	float		_14, _24, _34, _44; 

	float& m( int x, int y )
	{
		return ( ( float* )( this ) )[ y * 4 + x ];
	}

	inline float& operator[]( int i )
	{
		return ( ( float* )( this ) )[ i ];
	}

	inline _mmdpiMatrix_ operator=( const _mmdpiMatrix_& m )
	{
		this->_11 = m._11;
		this->_21 = m._21;
		this->_31 = m._31;
		this->_41 = m._41;

		this->_12 = m._12;
		this->_22 = m._22;	
		this->_32 = m._32;
		this->_42 = m._42;
				
		this->_13 = m._13;
		this->_23 = m._23;
		this->_33 = m._33;
		this->_43 = m._43;
		
		this->_14 = m._14;
		this->_24 = m._24;
		this->_34 = m._34;
		this->_44 = m._44;

		return *this;
	}

	inline _mmdpiMatrix_ operator*( const _mmdpiMatrix_& m )
	{
		_mmdpiMatrix_	r;

		//	matrix.u.i_[x][y]

		//	１行目
		//	r.u.i_[x][y] = _[i][y] * _[x][j] // i == j
		r._11 = this->_11 * m._11 + this->_21 * m._12 + this->_31 * m._13 + this->_41 * m._14;
		r._21 = this->_11 * m._21 + this->_21 * m._22 + this->_31 * m._23 + this->_41 * m._24;
		r._31 = this->_11 * m._31 + this->_21 * m._32 + this->_31 * m._33 + this->_41 * m._34;
		r._41 = this->_11 * m._41 + this->_21 * m._42 + this->_31 * m._43 + this->_41 * m._44;
		
		//	２行目
		r._12 = this->_12 * m._11 + this->_22 * m._12 + this->_32 * m._13 + this->_42 * m._14;
		r._22 = this->_12 * m._21 + this->_22 * m._22 + this->_32 * m._23 + this->_42 * m._24;
		r._32 = this->_12 * m._31 + this->_22 * m._32 + this->_32 * m._33 + this->_42 * m._34;
		r._42 = this->_12 * m._41 + this->_22 * m._42 + this->_32 * m._43 + this->_42 * m._44;
		
		//	３行目
		r._13 = this->_13 * m._11 + this->_23 * m._12 + this->_33 * m._13 + this->_43 * m._14;
		r._23 = this->_13 * m._21 + this->_23 * m._22 + this->_33 * m._23 + this->_43 * m._24;
		r._33 = this->_13 * m._31 + this->_23 * m._32 + this->_33 * m._33 + this->_43 * m._34;
		r._43 = this->_13 * m._41 + this->_23 * m._42 + this->_33 * m._43 + this->_43 * m._44;

		//	４行目
		r._14 = this->_14 * m._11 + this->_24 * m._12 + this->_34 * m._13 + this->_44 * m._14;
		r._24 = this->_14 * m._21 + this->_24 * m._22 + this->_34 * m._23 + this->_44 * m._24;
		r._34 = this->_14 * m._31 + this->_24 * m._32 + this->_34 * m._33 + this->_44 * m._34;
		r._44 = this->_14 * m._41 + this->_24 * m._42 + this->_34 * m._43 + this->_44 * m._44;

		return r;
	}

	inline _mmdpiVector4d_ operator*( const _mmdpiVector4d_& v )
	{
		_mmdpiVector4d_	r;

		r.x = this->_11 * v.x + this->_12 * v.y + this->_13 * v.z + this->_14 * v.w;			
		r.y	= this->_21 * v.x + this->_22 * v.y + this->_23 * v.z + this->_24 * v.w;		
		r.z	= this->_31 * v.x + this->_32 * v.y + this->_33 * v.z + this->_34 * v.w;
		r.w	= this->_41 * v.x + this->_42 * v.y + this->_43 * v.z + this->_44 * v.w;

		return r;
	}

	inline _mmdpiMatrix_ transelation( float x, float y, float z )
	{
		this->_14 = x;
		this->_24 = y;
		this->_34 = z;
		return *this;
	}

	//	回転
	inline _mmdpiMatrix_ rotation( float x, float y, float z, float r )
	{
		float	sin_r = sin( r );
		float	cos_r = cos( r );
		float	c = ( 1 - cos_r );

		this->_11 = x * x * c + cos_r;
		this->_12 = x * y * c + z * sin_r;
		this->_13 = x * z * c - y * sin_r;
		this->_14 = 0.0f;

		this->_21 = y * x * c - z * sin_r;
		this->_22 = y * y * c + cos_r;
		this->_23 = y * z * c + x * sin_r;
		this->_24 = 0.0f;

		this->_31 = z * x * c + y * sin_r;
		this->_32 = z * y * c - x * sin_r;
		this->_33 = z * z * c + cos_r;
		this->_34 = 0.0f;

		this->_41 = 0;
		this->_42 = 0;
		this->_43 = 0;
		this->_44 = 1;

		return *this;
	}

	//	サラスの行列式による逆行列解法
	inline _mmdpiMatrix_ get_inverse( void )
	{
		_mmdpiMatrix_			r;
		float					idet, det;
		det = determinant();
		
		if( fabs( det ) < 1e-8f )
			return r;
		idet = 1.0f / det;
 
		r._11 = +sub_determinant( 0, 0 ) * idet;
		r._12 = -sub_determinant( 1, 0 ) * idet;
		r._13 = +sub_determinant( 2, 0 ) * idet;
		r._14 = -sub_determinant( 3, 0 ) * idet;

		r._21 = -sub_determinant( 0, 1 ) * idet;
		r._22 = +sub_determinant( 1, 1 ) * idet;
		r._23 = -sub_determinant( 2, 1 ) * idet;
		r._24 = +sub_determinant( 3, 1 ) * idet;

		r._31 = +sub_determinant( 0, 2 ) * idet;
		r._32 = -sub_determinant( 1, 2 ) * idet;
		r._33 = +sub_determinant( 2, 2 ) * idet;
		r._34 = -sub_determinant( 3, 2 ) * idet;
			
		r._41 = -sub_determinant( 0, 3 ) * idet;
		r._42 = +sub_determinant( 1, 3 ) * idet;
		r._43 = -sub_determinant( 2, 3 ) * idet;
		r._44 = +sub_determinant( 3, 3 ) * idet;

		return r;
 	}

	//	サラスの行列式で扱えるよう、3x3 の行列に変換
	inline float sub_determinant( const unsigned short x_index, const unsigned short y_index )
	{
		if( x_index > 3 || y_index > 3 )
			return 0;
 
		float	sub_matrix[ 3 ][ 3 ];
		int		src_y;
		int		src_x;

		float	sarrus;

		src_y = 0;
		for( int j = 0; j < 3; j ++ )	// y
		{
			src_y = ( j == y_index )? src_y + 1 : src_y ;
			src_x = 0;
			for( int i = 0; i < 3; i ++ )	// x
			{
				src_x = ( i == x_index )? src_x + 1 : src_x ;
				sub_matrix[ j ][ i ] = m( src_x, src_y );//m( src_y * 4 + src_x );
				src_x ++;
			}
			src_y ++;
		}

		sarrus		= sub_matrix[ 0 ][ 0 ] * sub_matrix[ 1 ][ 1 ] * sub_matrix[ 2 ][ 2 ]
					+ sub_matrix[ 0 ][ 1 ] * sub_matrix[ 1 ][ 2 ] * sub_matrix[ 2 ][ 0 ]
					+ sub_matrix[ 1 ][ 0 ] * sub_matrix[ 2 ][ 1 ] * sub_matrix[ 0 ][ 2 ]

					- sub_matrix[ 0 ][ 2 ] * sub_matrix[ 1 ][ 1 ] * sub_matrix[ 2 ][ 0 ]
					- sub_matrix[ 1 ][ 2 ] * sub_matrix[ 2 ][ 1 ] * sub_matrix[ 0 ][ 0 ]
					- sub_matrix[ 0 ][ 1 ] * sub_matrix[ 1 ][ 0 ] * sub_matrix[ 2 ][ 2 ]
			;

		return sarrus;
	}

	//	
	inline float determinant( void )
	{
		return (
			+ _11 * sub_determinant( 0, 0 )
			- _21 * sub_determinant( 1, 0 )
			+ _31 * sub_determinant( 2, 0 ) 
			- _41 * sub_determinant( 3, 0 )
			);
	}
	
	inline mmdpiVector3d get_transform( void )
	{
		mmdpiVector3d	v;
		
		v.x = _14;
		v.y = _24;
		v.z = _34;
		
		return v;
	}

	inline mmdpiQuaternion get_quaternion( void )
	{
		mmdpiQuaternion	q;

		float elem[ 4 ]; 
		elem[ 0 ] = + _11 - _22 - _33 + 1.0f;
		elem[ 1 ] = - _11 + _22 - _33 + 1.0f;
		elem[ 2 ] = - _11 - _22 + _33 + 1.0f;
		elem[ 3 ] = + _11 + _22 + _33 + 1.0f;

		unsigned int biggest_index = 0;
		for( int i = 1; i < 4; i ++ )
		{
			if( elem[ i ] > elem[ biggest_index ] )
				biggest_index = i;
		}

		if( elem[ biggest_index ] < 0.0f )
			return q;

		float value = sqrt( elem[ biggest_index ] ) * 0.5f;
		q[ biggest_index ] = value;
		float s = 0.25f / value;

		switch ( biggest_index ) 
		{
		case 0: // x
			q.y = ( _21 + _12 ) * s;
			q.z = ( _13 + _31 ) * s;
			q.w = ( _32 - _23 ) * s;
			break;
		case 1: // y
			q.x = ( _21 + _12 ) * s;
			q.z = ( _32 + _23 ) * s;
			q.w = ( _13 - _21 ) * s;
			break;
		case 2: // z
			q.x = ( _13 + _31 ) * s;
			q.y = ( _23 + _23 ) * s;
			q.w = ( _21 - _12 ) * s;
			break;
		case 3: // w
			q.x = ( _32 - _23 ) * s;
			q.y = ( _13 - _31 ) * s;
			q.z = ( _21 - _12 ) * s;
			break;
		}

		return q;
	}

	inline _mmdpiMatrix_ quaternion( const mmdpiQuaternion& qt )
	{
		float qx = qt.x;
		float qy = qt.y;
		float qz = qt.z;
		float qw = qt.w;

		float sx = qx * qx;
		float sy = qy * qy;
		float sz = qz * qz;
		float cx = qy * qz;
		float cy = qx * qz;
		float cz = qx * qy;
		float wx = qw * qx;
		float wy = qw * qy;
		float wz = qw * qz;

		this->_11 = 1.0f - 2.0f * ( sy + sz );
		this->_21 = 2.0f * ( cz + wz );
		this->_31 = 2.0f * ( cy - wy );
		
		this->_12 = 2.0f * ( cz - wz );
		this->_22 = 1.0f - 2.0f * ( sx + sz );
		this->_32 = 2.0f * ( cx + wx );
		
		this->_13 = 2.0f * ( cy + wy );
		this->_23 = 2.0f * ( cx - wx );
		this->_33 = 1.0f - 2.0f * ( sx + sy );

		return *this;
	}

	inline _mmdpiMatrix_ scale( float x, float y, float z )
	{
		_11 = x;
		_22 = y;
		_33 = z;

		return *this;
	}

	inline void initialize( void )
	{
		_12 = _13 = _14 = 0;
		_21 = _23 = _24 = 0;
		_31 = _32 = _34 = 0;
		_41 = _42 = _43 = 0;

		_11 = _22 = _33 = _44 = 1;
	}

	_mmdpiMatrix_()
	{
		initialize();
	}

} mmdpiMatrix, *mmdpiMatrix_ptr;

//#pragma pack( pop )	//アラインメント制御エンド

#define mmdpi_debug		\
{	\
	GLenum error_code = glGetError();	\
	if( error_code != GL_NO_ERROR )		\
		printf( "Error.%d\n", error_code );	\
										\
}
