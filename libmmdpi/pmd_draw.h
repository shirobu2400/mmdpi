
#include "mmdpi_model.h"
#include "pmd_analyze.h"
#include "pmd_ik.h"
#include "vmd.h"

#pragma once


class mmdpiPmdDraw : public mmdpiPmdAnalyze, public mmdpiPmdIk
{

private :

public :

	void draw( void );

	mmdpiPmdDraw();
	~mmdpiPmdDraw();
} ;
