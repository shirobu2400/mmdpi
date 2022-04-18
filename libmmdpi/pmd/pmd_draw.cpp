
#include "pmd_draw.hpp"


// 描画
void mmdpiPmdDraw::draw( void )
{
	// ik
	for( int i = 0; i < mmdpiPmdAnalyze::ik_num; i ++ )
		mmdpiPmdIk::ik_execute( &mmdpiPmdAnalyze::ik[ i ], mmdpiBone::bone, mmdpiPmdLoad::bone );

	// Make bone matrix
	mmdpiBone::update_global_matrix_all();

	// 物理演算
	if( this->bullet_flag )
		mmdpiBone::advance_time_physical( mmdpiModel::get_fps() );

	mmdpiModel::draw();
}

mmdpiPmdDraw::mmdpiPmdDraw()
{
}

mmdpiPmdDraw::~mmdpiPmdDraw()
{
}
