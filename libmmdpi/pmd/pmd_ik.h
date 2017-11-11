
#include "pmd_struct.h"
#include "../model/mmdpi_bone.h"

#ifndef		__MMDPI__PMD__IK__DEFINES__
#define		__MMDPI__PMD__IK__DEFINES__	( 1 )

class mmdpiPmdIk
{
protected :
	
public :
	int		ik_execute( MMDPI_PMD_IK_INFO_PTR ik, MMDPI_BONE_INFO_PTR bone, MMDPI_PMD_BONE_INFO_PTR pbone );	//	IK
};

#endif	//	__MMDPI__PMD__IK__DEFINES__