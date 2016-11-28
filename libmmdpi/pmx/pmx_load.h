
#include "../model/tools/get_bin.h"
#include "pmx_struct.h"

#pragma once

class mmdpiPmxLoad
{

protected :

	//	Hader
	MMDPI_PMX_HEADER				head;
	
	//	Vertex
	dword						vertex_num;
	MMDPI_PMX_VERTEX_PTR				vertex;
	
	//	Face
	dword						face_num;
	dword*						face;
	//MMDPI_PMX_FACE_PTR				face;

	//	Texture
	dword						texture_num;
	MMDPI_PMX_TEXTURE_PTR				texture;
	
	//	Material
	dword						material_num;
	MMDPI_PMX_MATERIAL_PTR				material;

	//	Bone
	dword						bone_num;
	MMDPI_PMX_BONE_INFO_PTR				bone;

	//	Morph
	dword						morph_num;
	MMDPI_PMX_MORPH_INFO_PTR			morph;

	//	Display
	dword						show_num;
	MMDPI_PMX_SHOW_FRAME_INFO_PTR			show;

	//	Bullet Infomaion
	//	剛体
	dword						p_rigid_num;
	MMDPI_PHYSICAL_RIGID_INFO_PTR			p_rigid;
	
	//	ジョイント
	dword						p_joint_num;
	MMDPI_PHYSICAL_JOINT_INFO_PTR			p_joint;

	// 関数

	int		reader( GetBin* buf );
	
	char		directory[ 0xff ];
	int		get_direcotory( const char *file_name );
	int		get_header( GetBin* buf );
	char*		bin_string( GetBin* buf );

	char*		text_buf( GetBin* buf, uint* length = 0 );
	
	//	文字バイト数
	uint		byte_one_length;
	
public :

	virtual int	load( const char *file_name );

	MMDPI_PMX_BONE_INFO_PTR	get_pmx_bone( void )
	{
		return bone;
	}

	mmdpiPmxLoad();
	~mmdpiPmxLoad();
} ;
