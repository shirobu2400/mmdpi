
#include "pmx_analyze.h"
#include "pmx_ik.h"

#pragma once


class mmdpiPmxDraw : public mmdpiPmxAnalyze, public mmdpiPmxIk
{

private :

	int						morph_exec( dword index, float rate );

public :

	void					draw( void );

	mmdpiPmxDraw();
	~mmdpiPmxDraw();
} ;
