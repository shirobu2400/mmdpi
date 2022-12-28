
#include "pmx_draw.hpp"

#ifndef		__MMDPI__PMX__DEFINES__
#define		__MMDPI__PMX__DEFINES__		( 1 )

class mmdpiPmx : public mmdpiPmxDraw
{
public:
	virtual int		load( const char* pmx_name );

	virtual void		draw( void );
	virtual void		set_bone_matrix( uint bone_index, mmdpiMatrix& matrix );
};

#endif	//	__MMDPI__PMX__DEFINES__
