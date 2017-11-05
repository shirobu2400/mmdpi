
#include "pmx_ik.h"


//	IK
int mmdpiPmxIk::ik_execute( MMDPI_BONE_INFO_PTR bone, MMDPI_PMX_BONE_INFO_PTR pbone, int bone_index )
{
	const int	_ik_range_ = 255;
	const float	bottom_noise = 1e-6f;
	
	if( pbone[ bone_index ].ik_flag == 0 )
		return -1;
	
	MMDPI_PMX_BONE_INFO_PTR		npb	= &pbone[ bone_index ];
	MMDPI_BONE_INFO_PTR		nb	= &bone [ bone_index ];

	int		ik_link_num = ( signed )npb->ik_link_num;
	uint		iteration_num = npb->ik_loop_num;	//	

	iteration_num = ( iteration_num > _ik_range_ )? _ik_range_ : iteration_num ;

	mmdpiVector4d		v0001( 0, 0, 0, 1 );
			
	mmdpiVector4d		effect_pos_base	= mmdpiBone::get_global_matrix( nb ) * v0001;	//	IKの目指す目標位置	Effector
					
	for( uint j = 0; j < iteration_num; j ++ )
	{
		float	rotation_distance = 0;	//	移動した距離
	
		for( int i = 0; i < ik_link_num; i ++ )
		{		
			mmdpiMatrix		rotation_matrix;

			MMDPI_PMX_IK_INFO_PTR	my_ik			= &npb->ik_link[ i ];
			dword			attention_index		= my_ik->ik_bone_index;

			MMDPI_BONE_INFO_PTR	target_bone		= &bone[ attention_index ];			//	IKで目標を目指すボーン	Target
			MMDPI_BONE_INFO_PTR	ik_target_bone		= &bone[ npb->ik_target_bone_index ];
			mmdpiVector4d		target_pos_base		= mmdpiBone::get_global_matrix( ik_target_bone ) * v0001;	// Target
			
			mmdpiMatrix		local_mat		= mmdpiBone::get_global_matrix( target_bone );
			mmdpiMatrix		inv_coord		= local_mat.get_inverse();

			mmdpiVector4d		local_effect_pos;		//	Effector
			mmdpiVector4d		local_target_pos;		//	Target

			//	回転軸
			mmdpiVector3d		axis;
			float			radius_range = 1;


			//	ローカル座標系へ変換
			local_effect_pos = inv_coord * effect_pos_base;
			local_target_pos = inv_coord * target_pos_base;

			//mmdpiVector4d	diff_pos = local_effect_pos - local_target_pos;
			//if( diff_pos.dot( diff_pos ) < bottom_noise )
			//	continue;

			mmdpiVector3d		local_effect_dir( local_effect_pos.x, local_effect_pos.y, local_effect_pos.z );
			mmdpiVector3d		local_target_dir( local_target_pos.x, local_target_pos.y, local_target_pos.z );
	
			local_effect_dir.normalize();
			local_target_dir.normalize();

			//	向かう度合
			float	p = local_effect_dir.dot( local_target_dir );
			if( 1 < p )			
				p = 1;	//	arccos error!

			float	angle = acos( p );
			if( angle > +npb->ik_radius_range ) 
				angle = +npb->ik_radius_range, radius_range = 0;
			if( angle < -npb->ik_radius_range ) 
				angle = -npb->ik_radius_range, radius_range = 0;		

			if( npb->const_axis_flag )
			{
				//	回転軸制御（ボーン指定）
				for( int k = 0; k < 3; k ++ )
					axis[ k ] = npb->axis_vector[ k ];
			}
			else
			{
				axis = local_effect_dir.cross( local_target_dir );
				if( axis.dot( axis ) < 1e-16f )	//	axis is zero vector.
					break;
				axis.normalize();
			}
			
			//	Rotation matrix create.
			rotation_matrix.rotation( axis.x, axis.y, axis.z, angle );
			
			//	回転軸制御（IK上限）
			if( my_ik && my_ik->rotate_limit_flag )
			{
				rotation_range( &rotation_matrix,
					( mmdpiVector3d_ptr )my_ik->top, 
					( mmdpiVector3d_ptr )my_ik->bottom,
					npb->ik_radius_range );
			}
			
			//	反映
			target_bone->bone_mat = rotation_matrix * target_bone->bone_mat;
			
			//	移動した距離を計算
			rotation_distance += fabs( angle ) * radius_range;
		}

		//	インバースキネマティクスの補完が必要なくなった(反映する距離が小さい場合)
		if( rotation_distance < 1e-4f )
			break;
	}
	
	return 0;
}

//	回転制御
int mmdpiPmxIk::rotation_range( mmdpiMatrix_ptr rotation_matrix, mmdpiVector3d_ptr maxv, mmdpiVector3d_ptr minv, float once_range )
{
	mmdpiMatrix		rot_y, rot_x, rot_z;	//	Yow Pitch Roll

	////////////////////////
	// 軸回転角度を算出
	/////

	// X軸回り
	float fXLimit	= 80.0f / 180.0f * 3.14159f;
	float fSX	= -rotation_matrix->_32;    // sin(θx)
	float fX	= ( float )asin( fSX );   // X軸回り決定
	float fCX	= ( float )cos( fX );

	// ジンバルロック回避
	if( fabs( fX ) > fXLimit )
	{
		fX	= ( fX < 0 )? -fXLimit : fXLimit;
		fCX	= ( float )cos( fX );
	}

	// Y軸回り
	float fSY	= rotation_matrix->_31 / fCX;
	float fCY	= rotation_matrix->_33 / fCX;
	float fY	= ( float )atan2( fSY, fCY );   // Y軸回り決定

	// Z軸回り
	float fSZ	= rotation_matrix->_12 / fCX;
	float fCZ	= rotation_matrix->_22 / fCX;
	float fZ	= ( float )atan2( fSZ, fCZ );

	// 角度の制限
	//if( fX < minv->x )
	//	fX = minv->x;
	//if( fX > maxv->x )
	//	fX = maxv->x;
	
	if( fX < -maxv->x )
		fX = -maxv->x;
	if( fX > -minv->x )
		fX = -minv->x;

	if( fY < minv->y ) 
		fY = minv->y;
	if( fY > maxv->y ) 
		fY = maxv->y;

	//if( fZ < minv->z ) 
	//	fZ = minv->z;
	//if( fZ > maxv->z )
	//	fZ = maxv->z;

	if( fZ < -maxv->z )
		fZ = -maxv->z;
	if( fZ > -minv->z )
		fZ = -minv->z;

	if( fabs( fX ) > once_range )
		fX = once_range * ( ( fX < 0 )? -1 : +1 );
	
	if( fabs( fY ) > once_range )
		fY = once_range * ( ( fY < 0 )? -1 : +1 );

	if( fabs( fZ ) > once_range )
		fZ = once_range * ( ( fZ < 0 )? -1 : +1 );

	rot_x.rotation( 1, 0, 0, fX );
	rot_y.rotation( 0, 1, 0, fY );
	rot_z.rotation( 0, 0, 1, fZ );
	
	( *rotation_matrix ) = rot_z * rot_y * rot_x;

	return 0;
}
