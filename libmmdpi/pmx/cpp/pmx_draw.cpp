
#include "../h/pmx_draw.h"

void mmdpiPmxDraw::draw( void )
{
	//	ik
	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
		ik_execute( mmdpiBone::bone, mmdpiPmxLoad::bone, i );
	
	mmdpiModel::draw();
}

int mmdpiPmxDraw::morph_exec( dword index, float rate )
{
	MMDPI_PMX_MORPH_INFO_PTR	m = &morph[ index ];

	if( m->morph_flag == 1 )
	{
		m->morph_step = rate;
		m->morph_flag = 2;
	}
	if( m->morph_flag )
		m->morph_step += -0.02f;
	if( m->morph_step < 0.0f )
	{
		m->morph_step = 0;
		m->morph_flag = 0;
		morph_exec( index, 0.0f );
		//skin_update( m->vertex->vertex_id );
	}

	switch( m->type )
	{
	case 0:	//	グループ
		for( uint i = 0; i < m->offset_num; i ++ )
		{
			morph[ m->group[ i ].group_id ].morph_flag = 1;
			morph_exec( m->group[ i ].group_id, m->group[ i ].morph_rate );
		}
		break;
	case 1:	//	頂点
		for( uint i = 0; i < m->offset_num; i ++ )
		{
			//for( int j = 0; j < 3; j ++ )
			//	mmdpiShader::a_vertex_p->skinv[ m->vertex[ i ].vertex_id ][ j ] = m->morph_step * m->vertex[ i ].vertex[ j ];
			//skin_update( m->vertex[ i ].vertex_id, 0 );
		}
		break;
	case 2:	//	ボーン
		for( uint i = 0; i < m->offset_num; i ++ )
		{
			mmdpiMatrix		trans_matrix, rotate_matrix;
			rotate_matrix.rotation( 1, 0, 0, m->morph_step * m->bone[ i ].rotation[ 0 ] );
			rotate_matrix.rotation( 0, 1, 0, m->morph_step * m->bone[ i ].rotation[ 1 ] );
			rotate_matrix.rotation( 0, 0, 1, m->morph_step * m->bone[ i ].rotation[ 2 ] );

			trans_matrix
				.transelation(
				m->morph_step * m->bone[ i ].translate[ 0 ],
				m->morph_step * m->bone[ i ].translate[ 1 ], 
				m->morph_step * m->bone[ i ].translate[ 2 ] 
			);

			mmdpiBone::bone[ m->bone->bone_id ].matrix
				= rotate_matrix * trans_matrix * mmdpiBone::bone[ m->bone->bone_id ].matrix;
		}
		break;
	case 3:	//	ＵＶ
	case 4:	//	追加ＵＶ
	case 5:
	case 6:
	case 7:
	case 8:	//	材質
		break;
	}

	return 0;
}

mmdpiPmxDraw::mmdpiPmxDraw()
{
}

mmdpiPmxDraw::~mmdpiPmxDraw()
{
}
