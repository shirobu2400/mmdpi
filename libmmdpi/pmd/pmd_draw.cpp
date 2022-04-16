
#include "pmd_draw.hpp"


// 描画
void mmdpiPmdDraw::draw( void )
{
	//	ik
	for( int i = 0; i < mmdpiPmdAnalyze::ik_num; i ++ )
		ik_execute( &mmdpiPmdAnalyze::ik[ i ], mmdpiBone::bone, mmdpiPmdLoad::bone );

	//	Make bone matrix
	this->global_matrix();

	//	物理演算
	if( bullet_flag )
		this->advance_time_physical( mmdpiModel::get_fps() );

	mmdpiModel::draw();
}

mmdpiPmdDraw::mmdpiPmdDraw()
{
}

mmdpiPmdDraw::~mmdpiPmdDraw()
{
}
