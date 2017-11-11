
#include "pmd/pmd.h"
#include "pmx/pmx.h"
#include "vmd/vmd.h"

#ifdef	_WIN32
#define		__MMDPI__SJIS__FLAG__	( 1 )
#else
#define		__MMDPI__SJIS__FLAG__	( 0 )
#endif

#ifndef		__MMDPI__DEFINES__
#define		__MMDPI__DEFINES__	( 1 )

class mmdpi
{
protected :

	mmdpiModel*			pmm;
	mmdpiPmx*			pmx;
	mmdpiPmd*			pmd;
	vector<mmdpiVmd*>		vmd;
	
public :

	virtual int		load( const char* model_name );
	virtual int		vmd_load( const char *file_name );
	virtual mmdpiVmd*	get_vmd( int index );

	virtual void		draw( void );
	virtual void		set_bone_matrix( uint bone_index, mmdpiMatrix& matrix );
	virtual void		set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix );	// set bone matrix
	virtual	int		get_bone_num( void );
	virtual	char*		get_bone_name( int index, int coding_is_sjis = __MMDPI__SJIS__FLAG__ );
	virtual void		set_fps( int fps );
	virtual void		set_projection_matrix( const GLfloat* p_projection_matrix );	
	virtual void		set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix );

	mmdpi();
	~mmdpi();
} ;

#endif	//	__MMDPI__DEFINES__