
#include "pmx_draw.h"

class mmdpiPmx : public mmdpiPmxDraw
{
public :
	virtual int		load( const char* pmx_name );

	virtual void		draw( void );
	virtual void		set_bone_matrix( uint bone_index, mmdpiMatrix& matrix );
} ;