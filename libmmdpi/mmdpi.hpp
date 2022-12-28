#include "pmd/pmd.hpp"
#include "pmx/pmx.hpp"
#include "vmd/vmd.hpp"
#include <string>

#ifdef	_WINDOWS
#define		__MMDPI__SJIS__FLAG__	( 1 )
#else
#define		__MMDPI__SJIS__FLAG__	( 0 )
#endif

#ifndef		__MMDPI__DEFINES__
#define		__MMDPI__DEFINES__	( 1 )

class mmdpi
{
protected:
	mmdpiModel*				pmm;
	mmdpiPmx*				pmx;
	mmdpiPmd*				pmd;
	std::map<std::string, mmdpiVmd*>	vmd_instances;

public:
	virtual int		load( std::string model_name );
	virtual int		motion_load( std::string motion_name, std::string file_name );
	virtual int		vmd_load( std::string motion_name, std::string file_name );

	virtual mmdpiVmd*	vmd( std::string name );
	virtual mmdpiVmd*	get_vmd( std::string name );

	virtual void		draw( void );

	virtual void		set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix );
	virtual void		set_bone_matrix( std::string bone_name, const mmdpiMatrix& matrix );	// set bone matrix
	virtual	int		get_bone_num( void );
	virtual	char*		get_bone_name( int index, int coding_is_sjis = __MMDPI__SJIS__FLAG__ );

	virtual void		set_fps( int fps );
	virtual void		set_projection_matrix( const GLfloat* p_projection_matrix );
	virtual void		set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix );

	mmdpi();
	~mmdpi();
};

#endif	//	__MMDPI__DEFINES__
