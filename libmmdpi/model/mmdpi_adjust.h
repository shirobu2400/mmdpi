
#include "mmdpi_include.h"
#include "mmdpi_struct.h"

#pragma once

/*
 *	頂点の調節部
 *	
 *	以下、このプログラム共通で使用する各種データ型
 */

class mmdpiAdjust
{
private :

	dword				b_vertex_num;
	MMDPI_VERTEX_PTR		b_vertex;
	//MMDPI_BLOCK_VERTEX_PTR	b_vertex;
	
	dword				b_face_num;
	MMDPI_BLOCK_FACE_PTR		b_face;
	
	dword				b_material_num;
	MMDPI_MATERIAL_PTR		b_material;

	int				vertex_bone_over_range( uint* dev_pos, uint* using_bone_num, uint start, uint end, uint range,
								uint bone_num, dword* face, MMDPI_BLOCK_VERTEX_PTR vertex );

	dword				material_booking_vertex( vector<MMDPI_VERTEX*>* new_vertex, dword v, dword* face, dword material_index );

public :

	int				adjust_polygon( dword* face, dword face_num, MMDPI_BLOCK_VERTEX_PTR vertex, dword vertex_num );
	int				adjust_material_bone( dword material_num, MMDPI_MATERIAL_PTR material, dword bone_num, dword* face, MMDPI_BLOCK_VERTEX_PTR vertex );
	int				adjust_face( dword* face, dword face_num, dword vertex_num );
	int				adjust_bone( void );
	void				update_matrix( MMDPI_BONE_INFO_PTR bone, dword bone_num );
	 
	MMDPI_VERTEX_PTR get_vertex( void )
	{
		return b_vertex;
	}
	dword get_material_num( void )
	{
		return b_material_num;
	}
	MMDPI_MATERIAL_PTR get_material( void )
	{
		return b_material;
	}

	dword get_face_num( void )
	{
		return b_face_num;
	}
	MMDPI_BLOCK_FACE_PTR get_face_block( void )
	{
		return b_face;
	}

	mmdpiAdjust();
	~mmdpiAdjust();
};