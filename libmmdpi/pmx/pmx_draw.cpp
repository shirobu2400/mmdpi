
#include "pmx_draw.h"


void mmdpiPmxDraw::draw( void )
{
	for( int bone_update_counter = 0; bone_update_counter < 2; bone_update_counter ++ )
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

					//	matrix
					bone->matrix = make_global_matrix( i );
				}
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
	MMDPI_BONE_INFO_PTR	target_bone		= &bone[ bone_index ];
	float			rate			= pbone[ bone_index ].grant_parent_rate;


	if( !( pbone[ bone_index ].rotation_grant_flag || pbone[ bone_index ].translate_grant_flag ) )
		return 0;

	grant_bone = &bone[ grant_bone_index ];

	if( target_bone->level - grant_bone->level > 1 )
		return 0;

	if( rate < 0 )
		rate = 0;
	if( rate > 1 )
		rate = 1;

	if( pbone[ bone_index ].rotation_grant_flag )
	{
		mmdpiQuaternion		q1 = target_bone->bone_mat.get_quaternion();
		mmdpiQuaternion		q2 = grant_bone->bone_mat.get_quaternion();
		mmdpiQuaternion		q;
		q.slerp_quaternion( q1, q2, rate );
		target_bone->bone_mat.quaternion( q );
	}

	if( pbone[ bone_index ].translate_grant_flag )
	{
		mmdpiVector3d		p = grant_bone->bone_mat.get_transform();
		target_bone->bone_mat.transelation( p.x * rate, p.y * rate, p.z * rate );
	}

	//if( pbone[ bone_index ].translate_grant_flag )
	//{
	//	mmdpiVector4d		v0001( 0, 0, 0, 1 );
	//
	//	mmdpiVector4d		effect_pos_base		= mmdpiBone::get_global_matrix( grant_bone  ) * v0001;
	//	mmdpiVector4d		target_pos_base		= mmdpiBone::get_global_matrix( target_bone ) * v0001;
	//	
	//	mmdpiMatrix		local_mat		= mmdpiBone::get_global_matrix( grant_bone->parent );
	//	mmdpiMatrix		inv_coord		= local_mat.get_inverse();

	//	mmdpiVector4d		local_effect_pos;		//	Effector
	//	mmdpiVector4d		local_target_pos;		//	Target

	//	//	ローカル座標系へ変換
	//	//local_effect_pos = inv_coord * effect_pos_base;
	//	//local_target_pos = inv_coord * target_pos_base;

	//	local_effect_pos = effect_pos_base;
	//	local_target_pos = target_pos_base;

	//	mmdpiVector3d		local_effect_dir( local_effect_pos.x, local_effect_pos.y, local_effect_pos.z );
	//	mmdpiVector3d		local_target_dir( local_target_pos.x, local_target_pos.y, local_target_pos.z );

	//	float			angle;
	//	mmdpiVector3d		axis;
	//	mmdpiMatrix		rotation_matrix;

	//	local_effect_dir.normalize();
	//	local_target_dir.normalize();

	//	//	向かう度合
	//	float	p = local_effect_dir.dot( local_target_dir );
	//	if( 1 < p )			
	//		return 0;	//	arccos error!
	//	angle = acos( p ) * pbone[ bone_index ].grant_parent_rate;

	//	axis = local_effect_dir.cross( local_target_dir );
	//	axis.normalize();
	//		
	//	//	Rotation matrix create.
	//	rotation_matrix.rotation( axis.x, axis.y, axis.z, angle );

	//	target_bone->bone_mat = rotation_matrix * target_bone->bone_mat;
	//}

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
