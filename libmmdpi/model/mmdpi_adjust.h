
#include "mmdpi_include.h"
#include "mmdpi_struct.h"
#include "mmdpi_mesh.h"


#ifndef		__MMDPI__ADJUST__DEFINES__
#define		__MMDPI__ADJUST__DEFINES__	( 1 )

/*
 *	頂点の調節部
 *	
 *	以下、このプログラム共通で使用する各種データ型
 */

class mmdpiAdjust: public mmdpiShader
{
protected:

	//	区画表示
	vector<mmdpiMesh*>		mesh;

public:

	int				adjust( MMDPI_BLOCK_VERTEX* vertex, dword vertex_num, 
						dword* face, dword face_num,
						MMDPI_MATERIAL_PTR material, dword material_num,
						MMDPI_BONE_INFO_PTR bone, dword bone_num );
	mmdpiAdjust();
	~mmdpiAdjust();
};

#endif	//	__MMDPI__ADJUST__DEFINES__