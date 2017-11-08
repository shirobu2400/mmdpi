
#pragma once

#include "mmdpi_bone.h"


class mmdpiModel : public mmdpiBone
{
private :
	int				is_pmd;
	int				fps;

	mmdpiMatrix			projection_matrix;

	map<const string, uint>		bone_name2index_sjis;		// bone 名 to index
	map<const string, uint>		bone_name2index_utf8;		// bone 名 to index
	string*				bone_name;
	
	int				option_enable( void );
	int				option_disable( void );

public :
	virtual int			create( void );
	virtual int			set_bone_name2index( void );

	virtual void			draw( void );
	virtual void			set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix );		// set bone matrix
	virtual void			set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix );	// set bone matrix

	virtual	int			get_bone_num( void );
	virtual	char*			get_bone_name( int index, int coding_is_sjis = 0 );

	virtual void			set_projection_matrix( const GLfloat* p_projection_matrix );	
	virtual void			set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix );

	virtual int			set_physics_engine( int type );

	virtual void			set_pmx( void );

	virtual int			set_fps( int fps );
	virtual int			get_fps( void );

	mmdpiModel();
	~mmdpiModel();
} ;