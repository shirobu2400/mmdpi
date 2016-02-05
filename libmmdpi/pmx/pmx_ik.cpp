
#include "pmx_ik.h"


//	IK
int mmdpiPmxIk::ik_execute( MMDPI_BONE_INFO_PTR bone, MMDPI_PMX_BONE_INFO_PTR pbone, int bone_index )
{
	const int	_ik_range_ = 255;
	const float	bottom_noise = 1e-8f;

	if( pbone[ bone_index ].ik_flag == 0 )
		return -1;
	
	MMDPI_PMX_BONE_INFO_PTR		npb	= &pbone[ bone_index ];
	MMDPI_BONE_INFO_PTR			nb	= &bone[ bone_index ];

	int			ik_link_num = ( signed )npb->ik_link_num;
	uint		_iteration_num_ = npb->ik_loop_num;	//	

	_iteration_num_ = ( _iteration_num_ > _ik_range_ )? _ik_range_ : _iteration_num_ ;

	mmdpiVector4d	v0( 0, 0, 0, 1 );

	mmdpiVector4d	effect_pos_base	= mmdpiBone::get_local_matrix( nb ) * v0;	//	Effector

	for( uint j = 0; j < _iteration_num_; j ++ )
	{
		float	rotation_distance = 0;	//	移動した距離
	
		for( int i = 0; i < ik_link_num; i ++ )
		{		
			mmdpiMatrix		rotation_matrix;

			MMDPI_PMX_IK_INFO_PTR	my_ik = ( i < 0 )? 0x00 : &npb->ik_link[ i ];
			dword					attention_index = ( my_ik )? my_ik->ik_bone_index : npb->ik_target_bone_index ;

			MMDPI_BONE_INFO_PTR		target_bone		= &bone[ attention_index ];
			mmdpiVector4d			target_pos_base	= mmdpiBone::get_local_matrix( &bone[ npb->ik_target_bone_index ] ) * v0;	// Target
			
			mmdpiMatrix		attention_localmat	= mmdpiBone::get_local_matrix( target_bone );
			mmdpiMatrix		inv_coord			= attention_localmat.get_inverse();

			mmdpiVector4d	effect_pos		= effect_pos_base;		//	Effector
			mmdpiVector4d	target_pos		= target_pos_base;		//	Target

			//effect_pos.w		= 1;
			//target_pos.w		= 1;
			
			effect_pos.normalize();
			target_pos.normalize();

			effect_pos		 = inv_coord * ( effect_pos );
			target_pos		 = inv_coord * ( target_pos );

			mmdpiVector4d	diff_pos = effect_pos - target_pos;
			if( diff_pos.dot( diff_pos ) < bottom_noise )
				continue;

			mmdpiVector3d effect_dir( effect_pos.x, effect_pos.y, effect_pos.z );
			mmdpiVector3d target_dir( target_pos.x, target_pos.y, target_pos.z );

			effect_dir.normalize();
			target_dir.normalize();

			float	p = effect_dir.dot( target_dir );
			if( 1 < p )
				p = 1;	// arccos error!

			float	angle = acos( p );
			if( angle > npb->ik_radius_range ) 
				angle = npb->ik_radius_range;
			if( angle < -npb->ik_radius_range ) 
				angle = -npb->ik_radius_range;

			mmdpiVector3d	axis;
			axis = effect_dir.cross( target_dir );
			
			//	回転軸制御（ボーン指定）
			if( npb->const_axis_flag )
			{
				for( int k = 0; k < 3; k ++ )
					axis[ k ] = npb->axis_vector[ k ];
			}
			
			if( axis.dot( axis ) < bottom_noise )	//	axis is zero vector.
				continue;			
			axis.normalize();
			
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
			rotation_distance += fabs( angle );
		}

		////	インバースキネマティクスの補完が必要なくなった(反映する距離が小さい場合)
		//if( rotation_distance < 1e-4f )
		//	return 0;
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
	float fXLimit = 80.0f / 180.0f * 3.14159f;
	float fSX	= -rotation_matrix->_32;    // sin(θx)
	float fX	= ( float )asin( fSX );   // X軸回り決定
	float fCX	= ( float )cos( fX );

	// ジンバルロック回避
	if( fabs( fX ) > fXLimit )
	{
		fX	= ( fX < 0 )? -fXLimit : fXLimit;
		fCX = ( float )cos( fX );
	}

	// Y軸回り
	float fSY = rotation_matrix->_31 / fCX;
	float fCY = rotation_matrix->_33 / fCX;
	float fY = ( float )atan2( fSY, fCY );   // Y軸回り決定

	// Z軸回り
	float fSZ	= rotation_matrix->_12 / fCX;
	float fCZ	= rotation_matrix->_22 / fCX;
	float fZ	= ( float )atan2( fSZ, fCZ );

	// 角度の制限
	//if( fX < minv->x )
	//	fX = minv->x;
	//if( fX > maxv->x )
	//	fX = maxv->x;
	//
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