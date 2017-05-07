
#include "pmx_struct.h"
#include "../model/mmdpi_bone.h"

#pragma once

class mmdpiPmxIk
{
protected :
	int		rotation_range( mmdpiMatrix_ptr rotation_matrix, mmdpiVector3d_ptr max, mmdpiVector3d_ptr min, float once_range );
	
public :
	int		ik_execute( MMDPI_BONE_INFO_PTR bone, MMDPI_PMX_BONE_INFO_PTR pbone, int bone_index );	//	IK
} ;