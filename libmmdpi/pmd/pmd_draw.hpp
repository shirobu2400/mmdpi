
#include "pmd_analyze.hpp"
#include "pmd_ik.hpp"

#ifndef		__MMDPI__PMD__DRAW__DEFINES__
#define		__MMDPI__PMD__DRAW__DEFINES__	( 1 )


class mmdpiPmdDraw : public mmdpiPmdAnalyze, public mmdpiPmdIk
{
public:

	void draw( void );

	mmdpiPmdDraw();
	~mmdpiPmdDraw();
};

#endif	//	__MMDPI__PMD__DRAW__DEFINES__
