
#include "../h/pmd_draw.h"


// 描画
void mmdpiPmdDraw::draw( void )
{
	//	ik
	for( int i = 0; i < mmdpiPmdAnalyze::ik_num; i ++ )
		ik_execute( &mmdpiPmdAnalyze::ik[ i ], mmdpiBone::bone, mmdpiPmdLoad::bone );
	
	mmdpiModel::draw();
}

mmdpiPmdDraw::mmdpiPmdDraw()
{
}

mmdpiPmdDraw::~mmdpiPmdDraw()
{
}
