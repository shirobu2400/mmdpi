
#include "../../model/tools/h/get_bin.h"
#include "pmd_struct.h"
#pragma once


class mmdpiPmdLoad
{

protected :

	//	Hader
	MMDPI_PMD_HEADER					head;
	
	//	Vertex
	dword								vertex_num;
	MMDPI_PMD_VERTEX_PTR				vertex;
	
	//	Face
	dword								face_num;
	dword*								face;

	//	Matreial
	dword								material_num;
	MMDPI_PMD_MATERIAL_PTR				material;

	//	Bone
	ushort								bone_num;
	MMDPI_PMD_BONE_INFO_PTR				bone;

	//	IK
	ushort								ik_num;
	MMDPI_PMD_IK_INFO_PTR				ik;

	//	Skin
	ushort								skin_num;
	MMDPI_PMD_SKIN_PTR					skin;
	MMDPI_PMD_SKIN_VERTEX_PTR			ver_skin;
	
	//	Disp Skin
	BYTE								skin_cnt_num;
	MMDPI_PMD_SKIN_NUMBER_PTR			skin_cnt;

	//	Bone name
	BYTE								bone_name_num;
	MMDPI_PMD_BONE_NAME_PTR				bone_name;

	//	Bone Numbers
	dword								bone_number_num;
	MMDPI_PMD_BONE_NUMBER_PTR			bone_number;

	//	English
	MMDPI_PMD_ENGLISH_HEADER				en_head;
	MMDPI_PMD_ENGLISH_BONE_NAME_PTR			en_bone_name;
	MMDPI_PMD_ENGLISH_SKIN_NAME_PTR			en_skin_name;
	MMDPI_PMD_ENGLISH_BONE_DISP_NAME_PTR	en_bone_disp_name;

	//	Toon
	MMDPI_PMD_TOON_NAME_PTR				toon_name;

	//	Bullet Infomaion
	//	剛体
	dword								p_rigid_num;
	MMDPI_PHYSICAL_RIGID_INFO_PTR		p_rigid;
	
	//	ジョイント
	dword								p_joint_num;
	MMDPI_PHYSICAL_JOINT_INFO_PTR		p_joint;

	// 関数

	int		reader( GetBin* buf );
	
	char	directory[ 0xff ];
	int		get_direcotory( const char *file_name );
	int		get_header( GetBin* buf );
	int		bin_string( GetBin* buf, char *str, int bufRange );
	int		padding( GetBin* buf );
	
	int		char_byte( const char* _string );

public :

	virtual int		load( const char *file_name );
		
	MMDPI_PMD_BONE_INFO_PTR	get_pmd_bone( void )
	{
		return bone;
	}
	mmdpiPmdLoad();
	~mmdpiPmdLoad();
} ;
