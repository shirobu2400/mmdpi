
#include "vmd.h"


int mmdpiVmd::advance_time( float time_scale )
{ 
	if( bone == NULL || now_motion == NULL )
		return -1;

	motion_time += time_scale; 

	for( int i = 0; i < bone_num; i ++ )	//	ボーンごとにモーションを見る
	{
		if( now_motion[ i ] == NULL )
			continue;

		//	いまのモーションポインタ
		MMDPI_VMD_MOTION_PTR vp = now_motion[ i ]->motion;	
		if( vp == NULL )	// 親ボーン座標系に変換
			continue;
	
		MMDPI_VMD_MOTION_PTR vpn = ( now_motion[ i ]->next )? now_motion[ i ]->next->motion : NULL;
		if( vpn == NULL )	// これ以上モーションがない -> 番兵に当たった
			continue;

		mmdpiMatrix	now_matrix, rot_matrix, pos_matrix;

		mmdpiQuaternion	next_qt( vpn->qx, vpn->qy, vpn->qz, vpn->qw );
		mmdpiVector3d	next_vec( vpn->vx, vpn->vy, vpn->vz );

		mmdpiQuaternion	now_qt( vp->qx, vp->qy, vp->qz, vp->qw );
		mmdpiVector3d	now_vec( vp->vx, vp->vy, vp->vz );
		
		float		time_f = 0.5f;
		if( 0 <= vp->FrameNo && vp->FrameNo <= vpn->FrameNo ) 
			time_f = ( motion_time - ( float )vp->FrameNo ) / ( float )( vpn->FrameNo - vp->FrameNo ); 
		
		int		interInde = 0;
		const float	_interpolation_div_ = 127.0f;
		mmdpiVector3d	s_vec;
		//float		radw;

		// 高度な補間

		// x
		s_vec.x = interpolate( ( float )vp->Interpolation[ 0 ] / _interpolation_div_, ( float )vp->Interpolation[ 4 ] / _interpolation_div_, 
					( float )vp->Interpolation[ 8 ] / _interpolation_div_, ( float )vp->Interpolation[ 12 ] / _interpolation_div_, 
					time_f ) * ( next_vec.x - now_vec.x );
		// y
		s_vec.y = interpolate( ( float )vp->Interpolation[ 1 ] / _interpolation_div_, ( float )vp->Interpolation[ 5 ] / _interpolation_div_, 
					( float )vp->Interpolation[ 9 ] / _interpolation_div_, ( float )vp->Interpolation[ 13 ] / _interpolation_div_, 
					time_f ) * ( next_vec.y - now_vec.y );
		// z
		s_vec.z = interpolate( ( float )vp->Interpolation[ 2 ] / _interpolation_div_, ( float )vp->Interpolation[ 6 ] / _interpolation_div_, 
					( float )vp->Interpolation[ 10 ] / _interpolation_div_, ( float )vp->Interpolation[ 14 ] / _interpolation_div_, 
					time_f ) * ( next_vec.z - now_vec.z );
		//// radw
		//radw = interpolate( ( float )vp->Interpolation[ 3 ] / _interpolation_div_, ( float )vp->Interpolation[ 7 ] / _interpolation_div_, 
		//			( float )vp->Interpolation[ 11 ] / _interpolation_div_, ( float )vp->Interpolation[ 15 ] / _interpolation_div_, 
		//			time_f );
		s_vec = s_vec + now_vec;

		//	平行移動
		pos_matrix.transelation( s_vec.x, s_vec.y, s_vec.z );

		//	クォータニオン球面線形補間
		mmdpiQuaternion		s_qt;
		s_qt.slerp_quaternion( now_qt, next_qt, time_f );
		rot_matrix.quaternion( s_qt );

		if( motion_time >= ( float )vpn->FrameNo )
			now_motion[ i ] = now_motion[ i ]->next;	//次のモーションへ

		//	親ボーンからみた姿勢行列に
		//	変更量 * 親ボーンからみた初期姿勢
		bone[ i ].bone_mat = ( rot_matrix * pos_matrix ) * bone[ i ].init_mat;
	}

#ifdef _MMDPI_USINGSKIN_
	if( now_skin )
	{
		if( now_skin->skin == 0x00 )
		{
			now_skin = now_skin->next;
		}
		if( ( float )now_skin->skin->FrameNo <= motion_time )
		{
			MMDPI_VMD_SKIN_INFO_LIST_PTR	skinl = now_skin;
			while( skinl )
			{
				if( skin )
					skin[ skinl->target_id ].skin_flag = 1;
				if( morph )
					morph[ skinl->target_id ].morph_flag = 1;
				
				skinl = skinl->brot;
			}
			now_skin = now_skin->next;
		}
	}
#endif
	return 0;
}

int mmdpiVmd::is_end( void )
{
	return ( float )max_frame <= motion_time;
}

int mmdpiVmd::init_motion( void )
{
	motion_time = 0;
	for( int i = 0; i < bone_num; i ++ )
		now_motion[ i ] = motion_line[ i ];

	return 0;
}

//	ベジエ曲線
float mmdpiVmd::interpolate( float x1, float y1, float x2, float y2, float x )
{
	//	ベジエ曲線を利用して補間する。
	//	3次方程式は2分法を利用。
	const int	_loop_len_ = 8;
	float		s = 0.5f;
	float		t = 0.5f;
	float		ft = x;
	float		dft = x;
	float		dd;

	////	二分法
	//for( int i = 0; i < _loop_len_; i ++ )
	//{
	//	ft  = ( 3.0f * s * s * t * x1 ) + ( 3.0f * s * t * t * x2 ) + ( t * t * t ) - x;
	//	if( fabs( ft ) < 1e-4f ) 
	//		break;
	//	if( ft < 0 )
	//		t += 1 / ( 4 << i );
	//	else
	//		t -= 1 / ( 4 << i );
	//	s = 1 - t;
	//}

	//return ( 3.0f * s * s * t * y1 ) + ( 3.0f * s * t * t * y2 ) + ( t * t * t );


	//	ニュートン法のほうが収束が速いのでニュートン法でとく。
	for( int i = 0; i < _loop_len_; i ++ )
	{
		ft  = ( 3.0f * s * s * t * x1 ) + ( 3.0f * s * t * t * x2 ) + ( t * t * t ) - x;	// f(t)
		dft = ( 3.0f * s * s * x1 ) + ( 3.0f * 2.0f * t * x2 ) + ( 3.0f * t * t ) - x;		// d f(t) / dt

		dd = ft / dft;
		if( fabs( dd ) < 1e-2f ) 
			break;
		t = t - dd;
		s = 1 - t;
	}

	return ( 3.0f * s * s * t * y1 ) + ( 3.0f * s * t * t * y2 ) + ( t * t * t );
}

int mmdpiVmd::set_bone( MMDPI_BONE_INFO_PTR bone )
{
	this->bone = bone;
	return 0;
}

//	リスト構築
int mmdpiVmd::analyze( void )
{
	int		bone_id;

	m_list = new MMDPI_VMD_INFO_LIST[ bone_num ];
	motion_line = new MMDPI_VMD_INFO_LIST_PTR[ bone_num ];
	now_motion = new MMDPI_VMD_INFO_LIST_PTR[ bone_num ];

	if( m_list == 0x00 || motion_line == 0x00 || now_motion == 0x00 )
		return -1;

	for( int i = 0; i < bone_num; i ++ )
	{
		m_list[ i ].motion = NULL;
		m_list[ i ].next = NULL;
		m_list[ i ].prev = NULL;

		motion_line[ i ] = NULL;

		now_motion[ i ] = NULL;
	}

	for( dword i = 0; i < vmd_info->motion_num; i ++ )
	{
		string str( vmd_info->motion[ i ].BoneName );
		//	インデックス
		bone_id = bone_name_to_index[ str ];
		if( bone_id < 1 )
			continue;
		insert_motion_list( bone_id - 1, &vmd_info->motion[ i ] );

		//	最大モーション数
		if( max_frame < vmd_info->motion[ i ].FrameNo )
			max_frame = vmd_info->motion[ i ].FrameNo;
	}

	//	Skin
	skin_line = new MMDPI_VMD_SKIN_INFO_LIST;
	if( skin_line == 0x00 )
		return -1;
	skin_line->next	= 0;
	skin_line->prev	= 0;
	skin_line->brot	= 0;
	skin_line->target_id = 0;
	skin_line->skin	= 0;
	for( dword i = 0; i < vmd_info->skin_num.Count; i ++ )
		insert_skin( &vmd_info->skin[ i ] );
	while( skin_line->prev )
		skin_line = skin_line->prev;
	now_skin = skin_line;

	return init_motion();
}

//	スキン
int mmdpiVmd::insert_skin( MMDPI_VMD_SKIN_PTR skin )
{
	int				bflag = 0;
	MMDPI_VMD_SKIN_INFO_LIST_PTR	ml = skin_line;

	//	Insert
	MMDPI_VMD_SKIN_INFO_LIST_PTR new_skin = new MMDPI_VMD_SKIN_INFO_LIST;
	if( new_skin == 0x00 )
		return -1;

	//	セット
	new_skin->skin		= skin;
	new_skin->target_id	= skin_name_to_index[ skin->SkinName ];
	
	new_skin->next = NULL;
	new_skin->prev = NULL;
	new_skin->brot = NULL;

	//	相互関係
	MMDPI_VMD_SKIN_INFO_LIST_PTR	next_l = NULL;
	MMDPI_VMD_SKIN_INFO_LIST_PTR	prev_l = NULL;
	
	//	モーションのフレーム毎の挿入位置までシーク
	while( ml && ml->skin && ml->skin->FrameNo < skin->FrameNo )
	{
		if( ml->skin->FrameNo == skin->FrameNo && ml->brot == 0x00 )
		{
			ml->brot	= new_skin;
			bflag		= 1;
		}
		prev_l = ml;
		ml = ml->next;
	}
	next_l = ( bflag && ml && ml->skin )? ml : NULL ;

	new_skin->next = next_l;
	new_skin->prev = prev_l;

	//	関係設定
	if( next_l && next_l->skin )
		next_l->prev = new_skin;
	if( prev_l && prev_l->skin )
		prev_l->next = new_skin;

	//	はじめと比較して早いフレームを最初のフレームに指定
	if( skin_line == NULL || skin_line->skin == NULL )
		skin_line = new_skin;
	//	それ以外は必要な場所に入れる
	else if( skin_line->skin->FrameNo > new_skin->skin->FrameNo )
		skin_line = new_skin;

	return 0;
}

//	リストインサート
int mmdpiVmd::insert_motion_list( int bone_index, MMDPI_VMD_MOTION_PTR insert_motion )
{
	MMDPI_VMD_INFO_LIST_PTR ml = motion_line[ bone_index ];

	//	Insert
	MMDPI_VMD_INFO_LIST_PTR new_motion = new MMDPI_VMD_INFO_LIST;

	//	セット
	new_motion->motion	= insert_motion;
	
	new_motion->next = NULL;
	new_motion->prev = NULL;

	//	相互関係
	MMDPI_VMD_INFO_LIST_PTR	next_l = NULL;
	MMDPI_VMD_INFO_LIST_PTR	prev_l = NULL;

//	モーションのフレーム毎の挿入位置までシーク
	while( ml && ml->motion && ml->motion->FrameNo < insert_motion->FrameNo )
	{
		prev_l = ml;
		ml = ml->next;
	}
	next_l = ( ml && ml->motion )? ml : NULL ;

	new_motion->next = next_l;
	new_motion->prev = prev_l;

	//	関係設定
	if( next_l && next_l->motion )
		next_l->prev = new_motion;
	if( prev_l && prev_l->motion )
		prev_l->next = new_motion;

	//	はじめと比較して早いフレームを最初のフレームに指定
	if( motion_line[ bone_index ] == NULL || motion_line[ bone_index ]->motion == NULL )
		motion_line[ bone_index ] = new_motion;
	//	それ以外は必要な場所に入れる
	else if( motion_line[ bone_index ]->motion->FrameNo > new_motion->motion->FrameNo )
		motion_line[ bone_index ] = new_motion;

	return 0;
}

//	map で名前TOボーンインデックスを作る
int mmdpiVmd::create_bone_map( MMDPI_PMD_BONE_INFO_PTR bone, int bone_num )
{
	this->bone_num = bone_num;

	for( int i = 0; i < bone_num; i ++ )
	{
		string str( bone[ i ].bone_name );
		if( bone_name_to_index.find( str ) == bone_name_to_index.end() )
			bone_name_to_index.insert( map< string, int >::value_type( str, i + 1 ) );
	}
	
	return 0;
}

////	map で名前TOボーンインデックスを作る
//int	mmdpiVmd::create_morph_map( MMDPI_PMD_SKIN_INFO_PTR skin, int skin_num )
//{
//	for( int i = 0; i < skin_num; i ++ )
//	{
//		string	str_deta( skin[ i ].skin_name );
//		if( skin_name_to_index.find( str_deta ) == skin_name_to_index.end() )
//			skin_name_to_index.insert( map< string, int >::value_type( str_deta, i + 1 ) );
//	}
//	this->skin = skin;
//	
//	return 0;
//}

//	map で名前TOボーンインデックスを作る
int mmdpiVmd::create_bone_map_pmx( MMDPI_PMX_BONE_INFO_PTR bone, int bone_num )
{
	this->bone_num = bone_num;

	for( int i = 0; i < bone_num; i ++ )
	{
		string	str_sjis( bone[ i ].sjis_name );
		string	str_deta( bone[ i ].name );
		if( bone_name_to_index.find( str_sjis ) == bone_name_to_index.end() )
			bone_name_to_index.insert( map< string, int >::value_type( str_sjis, i + 1 ) );

		if( bone_name_to_index.find( str_deta ) == bone_name_to_index.end() )
			bone_name_to_index.insert( map< string, int >::value_type( str_deta, i + 1 ) );
	}
	
	return 0;
}

//	map で名前TOボーンインデックスを作る
int mmdpiVmd::create_morph_map_pmx( MMDPI_PMX_MORPH_INFO_PTR morph, int morph_num )
{
	for( int i = 0; i < morph_num; i ++ )
	{
		string	str_sjis( morph[ i ].sjis_name );
		string	str_deta( morph[ i ].name );
		if( skin_name_to_index.find( str_sjis ) == skin_name_to_index.end() )
			skin_name_to_index.insert( map< string, int >::value_type( str_sjis, i + 1 ) );

		if( skin_name_to_index.find( str_deta ) == skin_name_to_index.end() )
			skin_name_to_index.insert( map< string, int >::value_type( str_deta, i + 1 ) );
	}
	this->morph = morph;
	
	return 0;
}

int mmdpiVmd::load( const char *filename )
{
	const char *_vmd_head_ = "Vocaloid Motion Data 0002";

	GetBin*	buf = new GetBin();
	if( buf == 0x00 || buf->load( filename ) )
		return -1;

	vmd_info = new MMDPI_VMD_INFO;
	if( vmd_info == 0x00 )
		return -1;

	//	ヘッダ
	buf->get_bin( &vmd_info->header, sizeof( MMDPI_VMD_MMDPI_PMD_HEADER ) );
	if( strncmp( vmd_info->header.VmdHeader, _vmd_head_, strlen( _vmd_head_ ) ) ) 
		return -1;	//違ったら

	//	モーション読み込み
	buf->get_bin( &vmd_info->motion_num, sizeof( dword ) );
	vmd_info->motion = new MMDPI_VMD_MOTION[ vmd_info->motion_num ];
	if( vmd_info->motion == 0x00 )
		return -1;
	for( dword i = 0; i < vmd_info->motion_num; i ++ )
	{
		buf->get_bin( &vmd_info->motion[ i ], sizeof( MMDPI_VMD_MOTION ) );
	}

	//	スキン読み込み
	buf->get_bin( &vmd_info->skin_num, sizeof( MMDPI_VMD_SKIN_COUNT ) );
	vmd_info->skin = new MMDPI_VMD_SKIN[ vmd_info->skin_num.Count ];
	if( vmd_info->skin == 0x00 )
		return -1;
	for( dword i = 0; i < vmd_info->skin_num.Count; i ++ )
	{
		buf->get_bin( &vmd_info->skin[ i ], sizeof( MMDPI_VMD_SKIN ) );
	}
	
	delete buf;

	//	解析
	return analyze();
}

mmdpiVmd::mmdpiVmd()
{
	vmd_info	= 0x00;
	bone_num	= 0;
	m_list		= 0x00;
	motion_line	= 0x00;

	motion_time	= 0;
	max_frame	= 0;
	now_motion	= 0;
	this->bone	= 0x00;

	skin_line	= 0x00;
	//skin		= 0x00;
	morph		= 0x00;
	skin_morph_num	= 0;
}

mmdpiVmd::~mmdpiVmd()
{
	if( vmd_info )
	{
		if( vmd_info->motion )
		{
			delete[] vmd_info->motion;
			vmd_info->motion = 0x00;
		}
		if( vmd_info->skin )
		{
			delete[] vmd_info->skin;
			vmd_info->skin = 0x00;
		}

		delete vmd_info; vmd_info = 0x00;
	}

	if( motion_line )
	{
		for( int i = 0; i < bone_num; i ++ )
		{
			MMDPI_VMD_INFO_LIST_PTR	ml = motion_line[ i ];
			while( ml )
			{
				MMDPI_VMD_INFO_LIST_PTR mml = ml->next;
				delete ml;
				ml = 0x00;
				ml = mml;
			}
			motion_line[ i ] = 0x00;
		}
		delete[] motion_line;
		motion_line = 0x00;
	}

	if( skin_line )
	{
		MMDPI_VMD_SKIN_INFO_LIST_PTR	ml = skin_line;
		while( ml )
		{
			MMDPI_VMD_SKIN_INFO_LIST_PTR mml = ml->next;
			delete ml;
			ml = NULL;
			ml = mml;
		}
		skin_line = NULL;
		delete skin_line;
	}

	delete[] now_motion; now_motion = 0x00;
	delete[] m_list; m_list = 0x00;
}
