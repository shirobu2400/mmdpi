
#pragma once

#include "mmdpi_bone.h"


class mmdpiModel : public mmdpiBone
{
private :
	
	int				fps;

	int				option_enable( void );
	int				option_disable( void );
	
	mmdpiMatrix			projection_matrix;

	// cull_flag : 表示面フラグ 表:1, 裏:0
	int				draw_main( int cull_flag );
	
	map<const string, uint>		bone_name2index_sjis;		// bone 名 to index
	map<const string, uint>		bone_name2index_utf8;		// bone 名 to index
	
public :
	virtual int			create( void );
	virtual int			set_bone_name2index( void );

	virtual void			draw( void );
	virtual void			set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix );		// set bone matrix
	virtual void			set_bone_matrix( const char* bone_name, const mmdpiMatrix& matrix );	// set bone matrix

	virtual void			set_projection_matrix( const GLfloat* p_projection_matrix );	
	virtual void			set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix );

	virtual int			set_physics_engine( int type );

	virtual int			set_fps( int fps );
	virtual int			get_fps( void );

	mmdpiModel();
} ;