
#include "pmd_ik.h"


//	IK
int mmdpiPmdIk::ik_execute( MMDPI_PMD_IK_INFO_PTR ik, MMDPI_BONE_INFO_PTR bone, MMDPI_PMD_BONE_INFO_PTR pbone )
{
	//const int _ik_range_ = 16;
	
	MMDPI_PMD_IK_INFO_PTR ik_one = ik;

	uint		_iteration_num_ = ik_one->iterations;	//	
//#ifdef _MMDPI_OPENGL_ES_DEFINES_
	//_iteration_num_ = ( _iteration_num_ > _ik_range_ )? _ik_range_ : _iteration_num_ ;
//#endif
	mmdpiVector4d	v0( 0, 0, 0, 1 ), v1( 0, 0, 0, 1 );
		
	mmdpiVector4d	effect_pos_base	= mmdpiBone::get_global_matrix( &bone[ ik_one->ik_bone_index ] ) * ( v0 );	// Effector
			
	for( uint j = 0; j < _iteration_num_; j ++ )
	{
		float	rotation_distance = 0;	//	移動した距離
	
		for( uint i = 0; i < ik_one->ik_chain_length; i ++ )
		{		
			ushort			attention_index = ik_one->ik_child_bone_index[ i ];

			mmdpiVector4d	target_pos_base	= mmdpiBone::get_global_matrix( &bone[ ik_one->ik_target_bone_index ] )	* ( v1 );	// Target
		
			mmdpiMatrix		attention_localmat	= mmdpiBone::get_global_matrix( &bone[ attention_index ] );
			mmdpiMatrix		inv_coord		= attention_localmat.get_inverse();

			mmdpiVector4d	effect_pos			= effect_pos_base;		// Effector
			mmdpiVector4d	target_pos			= target_pos_base;		// Target

			effect_pos.w			= 1;
			target_pos.w			= 1;
			effect_pos			= inv_coord * effect_pos;
			target_pos			= inv_coord * target_pos;

			mmdpiVector4d	diff_pos = ( effect_pos - target_pos );
			if( diff_pos.dot( diff_pos ) < 1e-8f )
				return 0;

			mmdpiVector3d effect_dir		( effect_pos.x, effect_pos.y, effect_pos.z );
			mmdpiVector3d target_dir		( target_pos.x, target_pos.y, target_pos.z );

			//if( strcmp( pbone[ attention_index ].bone_name + 2, "ひざ" ) == 0 )		//	
			//	Is knee?
			if( ( unsigned char )pbone[ attention_index ].bone_name[ 2 ] == 0x82 
				&& ( unsigned char )pbone[ attention_index ].bone_name[ 3 ] == 0xD0
				&& ( unsigned char )pbone[ attention_index ].bone_name[ 4 ] == 0x82
				&& ( unsigned char )pbone[ attention_index ].bone_name[ 5 ] == 0xB4 )
			{
				effect_dir.x = 0;
				target_dir.x = 0;
			}
			effect_dir.normalize();
			target_dir.normalize();

			float	p = effect_dir.dot( target_dir );
			if( 1 < p )
				p = 1;	// arccos error!

			float	angle = acos( p );
			if( fabs( angle ) < 1e-8f )
				continue;
			if( angle > 4.0f * ik_one->control_weight ) 
				angle = 4.0f * ik_one->control_weight;

			mmdpiVector3d	axis;
			axis = effect_dir.cross( target_dir );
			if( axis.dot( axis ) < 1e-8f )	//	axis is zero vector.
				continue;
			axis.normalize();
			mmdpiMatrix		rotation_matrix;

			//	Rotation matrix create.
			rotation_matrix.rotation( axis.x, axis.y, axis.z, angle );

			//	Is knee?
			if( ( unsigned char )pbone[ attention_index ].bone_name[ 2 ] == 0x82 
				&& ( unsigned char )pbone[ attention_index ].bone_name[ 3 ] == 0xD0
				&& ( unsigned char )pbone[ attention_index ].bone_name[ 4 ] == 0x82
				&& ( unsigned char )pbone[ attention_index ].bone_name[ 5 ] == 0xB4 )
			{
				mmdpiMatrix		inv_matrix = bone[ attention_index ].init_mat.get_inverse();
				mmdpiMatrix		def_matrix = rotation_matrix * bone[ attention_index ].bone_mat * inv_matrix;
				mmdpiVector4d	t_vec( 0, 0, 1, 1 );
				t_vec = def_matrix * ( t_vec );
				if( t_vec.y < 0.1f )
					rotation_matrix.rotation( axis.x, axis.y, axis.z, -angle );
			}
			
			//	反映
			bone[ attention_index ].bone_mat = rotation_matrix * bone[ attention_index ].bone_mat;

			//	移動した距離を計算
			rotation_distance += fabs( angle );
		}

		//	インバースキネマティクスの補完が必要なくなった(反映する距離が小さい場合)
		if( rotation_distance < 1e-8f )
			return 0;
	}

	return 0;
}
