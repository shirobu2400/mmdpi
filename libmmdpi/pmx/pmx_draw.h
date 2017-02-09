
#include "pmx_analyze.h"
#include "pmx_ik.h"

#pragma once


class mmdpiPmxDraw : public mmdpiPmxAnalyze, public mmdpiPmxIk
{

private :

	int		morph_exec( dword index, float rate );
	int		grant_bone( MMDPI_BONE_INFO_PTR bone, MMDPI_PMX_BONE_INFO_PTR pbone, int bone_index, MMDPI_BONE_INFO_PTR grant_bone );	//	付与ボーン

public :

	void		draw( void );

	mmdpiPmxDraw();
	~mmdpiPmxDraw();
} ;
