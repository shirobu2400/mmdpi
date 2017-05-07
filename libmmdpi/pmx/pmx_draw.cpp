
#include "pmx_draw.h"


void mmdpiPmxDraw::draw( void )
{
	for( uint level = 0; level < mmdpiPmxLoad::bone_level_range; level ++ )
	{	
		for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
		{
			MMDPI_PMX_BONE_INFO_PTR		bonep = &mmdpiPmxLoad::bone[ i ];
			MMDPI_BONE_INFO_PTR		bone  = &mmdpiBone::bone[ i ];
			if( bonep->level == level )
			{
				//	ik
				ik_execute( mmdpiBone::bone, mmdpiPmxLoad::bone, i );

				//	付与
				grant_bone( mmdpiBone::bone, mmdpiPmxLoad::bone, i, bonep->grant_parent_index );
			}
		}
	}		

	for( uint level = 0; level < mmdpiPmxLoad::bone_level_range; level ++ )
	{	
		for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
		{
			MMDPI_PMX_BONE_INFO_PTR		bonep = &mmdpiPmxLoad::bone[ i ];
			MMDPI_BONE_INFO_PTR		bone  = &mmdpiBone::bone[ i ];
			if( bonep->level == level )
			{
				//	ik
				ik_execute( mmdpiBone::bone, mmdpiPmxLoad::bone, i );

				//	matrix
				bone->matrix = make_global_matrix( i );
			}
		}
	}

	//	物理演算
	if( bullet_flag )	
		this->advance_time_physical( mmdpiModel::get_fps() );
	
	mmdpiModel::draw();
}

//	付与親ボーン処理
int mmdpiPmxDraw::grant_bone( MMDPI_BONE_INFO_PTR bone, MMDPI_PMX_BONE_INFO_PTR pbone, int bone_index, int grant_bone_index )
{
	MMDPI_BONE_INFO_PTR	grant_bone;
	MMDPI_BONE_INFO_PTR	target_bone;
	float			rate;


	if( !( pbone[ bone_index ].rotation_grant_flag || pbone[ bone_index ].translate_grant_flag ) )
		return 0;

	target_bone	= &bone[ bone_index ];
	grant_bone	= &bone[ grant_bone_index ];
	rate		= pbone[ bone_index ].grant_parent_rate;

	if( pbone[ bone_index ].rotation_grant_flag )
	{
		mmdpiVector3d			rotatev;
		mmdpiVector3d			rotate2;
		mmdpiMatrix			rotatem;
		grant_bone->bone_mat.get_rotation ( &rotate2.x, &rotate2.y, &rotate2.z );
		rotatev = rotate2 * rate;
		rotatem.rotation( rotatev.x, rotatev.y, rotatev.z );
		target_bone->bone_mat = rotatem * target_bone->bone_mat;
	}

	if( pbone[ bone_index ].translate_grant_flag )
	{
		mmdpiVector3d		position = grant_bone->bone_mat.get_transform();
		position = position * rate;
		target_bone->bone_mat.transelation( position.x, position.y, position.z );
	}

	return 0;
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
