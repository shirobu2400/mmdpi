
#include "pmd_struct.hpp"
#include "../model/mmdpi_bone.hpp"

#ifndef		__MMDPI__PMD__IK__DEFINES__
#define		__MMDPI__PMD__IK__DEFINES__	( 1 )

class mmdpiPmdIk
{
public:
	int ik_execute( MMDPI_PMD_IK_INFO_PTR ik, MMDPI_BONE_INFO_PTR bone, MMDPI_PMD_BONE_INFO_PTR pbone );	//	IK
};

#endif	//	__MMDPI__PMD__IK__DEFINES__
