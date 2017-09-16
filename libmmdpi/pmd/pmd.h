
#include "pmd_draw.h"

class mmdpiPmd : public mmdpiPmdDraw
{
public :
	virtual int			load( const char* pmd_name );

	virtual void		draw( void );
	virtual void		set_bone_matrix( uint bone_index, mmdpiMatrix& matrix );
} ;