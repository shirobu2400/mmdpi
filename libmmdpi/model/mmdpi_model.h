
#pragma once

#include "mmdpi_bone.h"


class mmdpiModel : public mmdpiBone
{
private :
	
	int				fps;

	int				OptionEnable( void );
	int				OptionDisable( void );
	
	mmdpiMatrix			projection_matrix;

	//	cull_flag : 表示面フラグ 表:1, 裏:0
	int				draw_main( int cull_flag );
	
public :
	virtual int			create( void );

	virtual void			draw( void );
	virtual void			set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix );

	virtual void			set_projection_matrix( const GLfloat* p_projection_matrix );	
	virtual void			set_projection_matrix( const mmdpiMatrix_ptr p_projection_matrix );

	virtual int			set_physics_engine( int type );

	virtual int			set_fps( int fps );
	virtual int			get_fps( void );

	mmdpiModel();
} ;