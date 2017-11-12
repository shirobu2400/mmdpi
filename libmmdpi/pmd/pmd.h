
#include "pmd_draw.h"


#ifndef		__MMDPI__PMD__DEFINES__
#define		__MMDPI__PMD__DEFINES__		( 1 )


class mmdpiPmd : public mmdpiPmdDraw
{
public:
	virtual int		load( const char* pmd_name );

	virtual void		draw( void );
	virtual void		set_bone_matrix( uint bone_index, mmdpiMatrix& matrix );
};

#endif	//	__MMDPI__PMD__DEFINES__