
#include "vmd.hpp"


int mmdpiVmd::advance_time( float time_scale )
{
	if( bone == 0x00 || now_motion == 0x00 )
		return -1;

	motion_time += time_scale;

	for( int i = 0; i < bone_num; i ++ )	// ボーンごとにモーションを見る
	{
		if( now_motion[ i ] == 0x00 )
			continue;

		// いまのモーションポインタ
		MMDPI_VMD_MOTION_PTR vp = now_motion[ i ]->motion;
		if( vp == 0x00 )	// 親ボーン座標系に変換
			continue;

		MMDPI_VMD_MOTION_PTR vpn = ( now_motion[ i ]->next )? now_motion[ i ]->next->motion : 0x00;
		if( vpn == 0x00 )	// これ以上モーションがない -> 番兵に当たった
			continue;

		mmdpiMatrix	now_matrix, rot_matrix, pos_matrix;

		mmdpiQuaternion	next_qt( vpn->qx, vpn->qy, vpn->qz, vpn->qw );
		mmdpiVector3d	next_vec( vpn->vx, vpn->vy, vpn->vz );

		mmdpiQuaternion	now_qt( vp->qx, vp->qy, vp->qz, vp->qw );
		mmdpiVector3d	now_vec( vp->vx, vp->vy, vp->vz );

		float		time_f = 0.5f;
		if( 0 <= vp->frame_number && vp->frame_number <= vpn->frame_number )
			time_f = ( motion_time - ( float )vp->frame_number ) / ( float )( vpn->frame_number - vp->frame_number );

		int		interInde = 0;
		const float	_interpolation_div_ = 127.0f;
		mmdpiVector3d	s_vec;
		//float		radw;

		// 高度な補間

		// x
		s_vec.x = interpolate( ( float )vp->interpolation[ 0 ] / _interpolation_div_, ( float )vp->interpolation[ 4 ] / _interpolation_div_,
					( float )vp->interpolation[ 8 ] / _interpolation_div_, ( float )vp->interpolation[ 12 ] / _interpolation_div_,
					time_f ) * ( next_vec.x - now_vec.x );
		// y
		s_vec.y = interpolate( ( float )vp->interpolation[ 1 ] / _interpolation_div_, ( float )vp->interpolation[ 5 ] / _interpolation_div_,
					( float )vp->interpolation[ 9 ] / _interpolation_div_, ( float )vp->interpolation[ 13 ] / _interpolation_div_,
					time_f ) * ( next_vec.y - now_vec.y );
		// z
		s_vec.z = interpolate( ( float )vp->interpolation[ 2 ] / _interpolation_div_, ( float )vp->interpolation[ 6 ] / _interpolation_div_,
					( float )vp->interpolation[ 10 ] / _interpolation_div_, ( float )vp->interpolation[ 14 ] / _interpolation_div_,
					time_f ) * ( next_vec.z - now_vec.z );
		//// radw
		//radw = interpolate( ( float )vp->interpolation[ 3 ] / _interpolation_div_, ( float )vp->interpolation[ 7 ] / _interpolation_div_,
		// 		( float )vp->interpolation[ 11 ] / _interpolation_div_, ( float )vp->interpolation[ 15 ] / _interpolation_div_,
		// 		time_f );

		s_vec = s_vec + now_vec;

		// 平行移動
		pos_matrix.transelation( s_vec.x, s_vec.y, s_vec.z );

		// クォータニオン球面線形補間
		mmdpiQuaternion		s_qt;
		s_qt.slerp_quaternion( now_qt, next_qt, time_f );
		rot_matrix.quaternion( s_qt );

		if( motion_time >= ( float )vpn->frame_number )
			now_motion[ i ] = now_motion[ i ]->next;	//次のモーションへ

		// 親ボーンからみた姿勢行列に
		// 変更量 * 親ボーンからみた初期姿勢
		bone[ i ].bone_mat = ( rot_matrix * pos_matrix ) * bone[ i ].init_mat;
	}

#ifdef _MMDPI_USINGSKIN_
	if( now_skin )
	{
		if( now_skin->skin == 0x00 )
		{
			now_skin = now_skin->next;
		}
		if( ( float )now_skin->skin->frame_number <= motion_time )
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
	return ( float )this->max_frame <= this->motion_time;
}

int mmdpiVmd::init_motion( void )
{
	this->motion_time = 0;
	for( int i = 0; i < bone_num; i ++ )
		this->now_motion[ i ] = this->motion_line[ i ];
	return 0;
}

// ベジエ曲線
float mmdpiVmd::interpolate( float x1, float y1, float x2, float y2, float x )
{
	// ベジエ曲線を利用して補間する。
	// 3次方程式は2分法を利用。
	const int	_loop_len_ = 8;
	float		s = 0.5f;
	float		t = 0.5f;
	float		ft = x;
	float		dft = x;
	//float		dd;

	// 二分法
	for( int i = 0; i < _loop_len_; i ++ )
	{
		ft = ( 3.0f * s * s * t * x1 ) + ( 3.0f * s * t * t * x2 ) + ( t * t * t ) - x;
		if( fabs( ft ) < 1e-4f )
			break;
		if( ft < 0 )
			t += 1.0f / ( 4 << i );
		else
			t -= 1.0f / ( 4 << i );
		s = 1 - t;
	}
	return ( 3.0f * s * s * t * y1 ) + ( 3.0f * s * t * t * y2 ) + ( t * t * t );


	//// ニュートン法のほうが収束が速いのでニュートン法でとく。
	//// が、おそらく、この数式はおかしい。実装のミス
	//for( int i = 0; i < _loop_len_; i ++ )
	//{
	// ft  = ( 3.0f * s * s * t * x1 ) + ( 3.0f * s * t * t * x2 ) + ( t * t * t ) - x;	// f(t)
	// dft = ( 3.0f * s * s * x1 ) + ( 3.0f * 2.0f * t * x2 ) + ( 3.0f * t * t ) - x;		// d f(t) / dt
	//
	// if( fabs( dft ) < 1e-6f )
	// 	break;

	// dd = ft / dft;

	// t = t - dd;
	// s = 1 - t;

	// if( fabs( dd ) < 1e-6f )
	// 	break;
	//}
	//return ( 3.0f * s * s * t * y1 ) + ( 3.0f * s * t * t * y2 ) + ( t * t * t );
}

int mmdpiVmd::set_bone( MMDPI_BONE_INFO_PTR bone )
{
	this->bone = bone;
	return 0;
}

// リスト構築
int mmdpiVmd::analyze( void )
{
	int		bone_id;

	this->m_list = new MMDPI_VMD_INFO_LIST[ this->bone_num ];
	this->motion_line = new MMDPI_VMD_INFO_LIST_PTR[ this->bone_num ];
	this->now_motion = new MMDPI_VMD_INFO_LIST_PTR[ this->bone_num ];

	if( this->m_list == 0x00 || this->motion_line == 0x00 || this->now_motion == 0x00 )
		return -1;

	for( int i = 0; i < bone_num; i ++ )
	{
		this->m_list[ i ].motion = 0x00;
		this->m_list[ i ].next = 0x00;
		this->m_list[ i ].prev = 0x00;

		this->motion_line[ i ] = 0x00;

		this->now_motion[ i ] = 0x00;
	}

	for( dword i = 0; i < this->vmd_info->motion_num; i ++ )
	{
		std::string 	str( this->vmd_info->motion[ i ].bone_name );
		// インデックス
		bone_id = this->bone_name_to_index[ str ];
		if( bone_id < 1 )
			continue;
		insert_motion_list( bone_id - 1, &vmd_info->motion[ i ] );

		// 最大モーション数
		if( this->max_frame < this->vmd_info->motion[ i ].frame_number )
			this->max_frame = this->vmd_info->motion[ i ].frame_number;
	}

	// Skin
	this->skin_line = new MMDPI_VMD_SKIN_INFO_LIST;
	if( this->skin_line == 0x00 )
		return -1;
	this->skin_line->next	= 0;
	this->skin_line->prev	= 0;
	this->skin_line->brot	= 0;
	this->skin_line->target_id = 0;
	this->skin_line->skin	= 0;
	for( dword i = 0; i < this->vmd_info->skin_num.count; i ++ )
		insert_skin( &this->vmd_info->skin[ i ] );
	while( this->skin_line->prev )
		this->skin_line = this->skin_line->prev;
	this->now_skin = this->skin_line;

	return this->init_motion();
}

// スキン
int mmdpiVmd::insert_skin( MMDPI_VMD_SKIN_PTR skin )
{
	int				bflag = 0;
	MMDPI_VMD_SKIN_INFO_LIST_PTR	ml = skin_line;

	// Insert
	MMDPI_VMD_SKIN_INFO_LIST_PTR new_skin = new MMDPI_VMD_SKIN_INFO_LIST;
	if( new_skin == 0x00 )
		return -1;

	// セット
	new_skin->skin		= skin;
	new_skin->target_id	= skin_name_to_index[ skin->skin_name ];

	new_skin->next = 0x00;
	new_skin->prev = 0x00;
	new_skin->brot = 0x00;

	// 相互関係
	MMDPI_VMD_SKIN_INFO_LIST_PTR	next_l = 0x00;
	MMDPI_VMD_SKIN_INFO_LIST_PTR	prev_l = 0x00;

	// モーションのフレーム毎の挿入位置までシーク
	while( ml && ml->skin && ml->skin->frame_number < skin->frame_number )
	{
		if( ml->skin->frame_number == skin->frame_number && ml->brot == 0x00 )
		{
			ml->brot	= new_skin;
			bflag		= 1;
		}
		prev_l = ml;
		ml = ml->next;
	}
	next_l = ( bflag && ml && ml->skin )? ml : 0x00 ;

	new_skin->next = next_l;
	new_skin->prev = prev_l;

	// 関係設定
	if( next_l && next_l->skin )
		next_l->prev = new_skin;
	if( prev_l && prev_l->skin )
		prev_l->next = new_skin;

	// はじめと比較して早いフレームを最初のフレームに指定
	if( skin_line == 0x00 || skin_line->skin == 0x00 )
		skin_line = new_skin;
	// それ以外は必要な場所に入れる
	else if( skin_line->skin->frame_number > new_skin->skin->frame_number )
		skin_line = new_skin;

	return 0;
}

// リストインサート
int mmdpiVmd::insert_motion_list( int bone_index, MMDPI_VMD_MOTION_PTR insert_motion )
{
	MMDPI_VMD_INFO_LIST_PTR ml = motion_line[ bone_index ];

	// Insert
	MMDPI_VMD_INFO_LIST_PTR new_motion = new MMDPI_VMD_INFO_LIST;

	// セット
	new_motion->motion	= insert_motion;

	new_motion->next = 0x00;
	new_motion->prev = 0x00;

	// 相互関係
	MMDPI_VMD_INFO_LIST_PTR	next_l = 0x00;
	MMDPI_VMD_INFO_LIST_PTR	prev_l = 0x00;

// モーションのフレーム毎の挿入位置までシーク
	while( ml && ml->motion && ml->motion->frame_number < insert_motion->frame_number )
	{
		prev_l = ml;
		ml = ml->next;
	}
	next_l = ( ml && ml->motion )? ml : 0x00 ;

	new_motion->next = next_l;
	new_motion->prev = prev_l;

	// 関係設定
	if( next_l && next_l->motion )
		next_l->prev = new_motion;
	if( prev_l && prev_l->motion )
		prev_l->next = new_motion;

	// はじめと比較して早いフレームを最初のフレームに指定
	if( motion_line[ bone_index ] == 0x00 || motion_line[ bone_index ]->motion == 0x00 )
		motion_line[ bone_index ] = new_motion;
	// それ以外は必要な場所に入れる
	else if( motion_line[ bone_index ]->motion->frame_number > new_motion->motion->frame_number )
		motion_line[ bone_index ] = new_motion;

	return 0;
}

// map で名前TOボーンインデックスを作る
int mmdpiVmd::create_bone_map( MMDPI_PMD_BONE_INFO_PTR bone, int bone_num )
{
	this->bone_num = bone_num;

	for( int i = 0; i < bone_num; i ++ )
	{
		std::string str( bone[ i ].bone_name );
		if( bone_name_to_index.find( str ) == bone_name_to_index.end() )
			bone_name_to_index.insert( std::map<std::string, int>::value_type( str, i + 1 ) );
	}

	return 0;
}

//// map で名前TOボーンインデックスを作る
//int	mmdpiVmd::create_morph_map( MMDPI_PMD_SKIN_INFO_PTR skin, int skin_num )
//{
// for( int i = 0; i < skin_num; i ++ )
// {
// 	std::string	str_deta( skin[ i ].skin_name );
// 	if( skin_name_to_index.find( str_deta ) == skin_name_to_index.end() )
// 		skin_name_to_index.insert( std::map<std::string, int>::value_type( str_deta, i + 1 ) );
// }
// this->skin = skin;
//
// return 0;
//}

// map で名前TOボーンインデックスを作る
int mmdpiVmd::create_bone_map_pmx( MMDPI_PMX_BONE_INFO_PTR bone, int bone_num )
{
	this->bone_num = bone_num;

	for( int i = 0; i < bone_num; i ++ )
	{
		std::string	str_sjis( bone[ i ].sjis_name );
		std::string	str_deta( bone[ i ].name );
		if( bone_name_to_index.find( str_sjis ) == bone_name_to_index.end() )
			bone_name_to_index.insert( std::map<std::string, int>::value_type( str_sjis, i + 1 ) );

		if( bone_name_to_index.find( str_deta ) == bone_name_to_index.end() )
			bone_name_to_index.insert( std::map<std::string, int>::value_type( str_deta, i + 1 ) );
	}

	return 0;
}

// map で名前TOボーンインデックスを作る
int mmdpiVmd::create_morph_map_pmx( MMDPI_PMX_MORPH_INFO_PTR morph, int morph_num )
{
	for( int i = 0; i < morph_num; i ++ )
	{
		std::string	str_sjis( morph[ i ].sjis_name );
		std::string	str_deta( morph[ i ].name );
		if( skin_name_to_index.find( str_sjis ) == skin_name_to_index.end() )
			skin_name_to_index.insert( std::map<std::string, int>::value_type( str_sjis, i + 1 ) );

		if( skin_name_to_index.find( str_deta ) == skin_name_to_index.end() )
			skin_name_to_index.insert( std::map<std::string, int>::value_type( str_deta, i + 1 ) );
	}
	this->morph = morph;

	return 0;
}

int mmdpiVmd::load( const char *filename )
{
	const char*	_vmd_head_ = "Vocaloid Motion Data 0002";
	GetBin*		buf = new GetBin();
	if( buf == 0x00 || buf->load( filename ) )
		return -1;

	this->vmd_info = new MMDPI_VMD_INFO;
	if( this->vmd_info == 0x00 )
		return -1;

	// ヘッダ
	buf->get_bin( &this->vmd_info->header, sizeof( MMDPI_VMD_MMDPI_PMD_HEADER ) );
	if( strncmp( this->vmd_info->header.vmd_header, _vmd_head_, strlen( _vmd_head_ ) ) )
		return -1;	// 違ったら

	// モーション読み込み
	buf->get_bin( &this->vmd_info->motion_num, sizeof( dword ) );
	this->vmd_info->motion = new MMDPI_VMD_MOTION[ this->vmd_info->motion_num ];
	if( this->vmd_info->motion == 0x00 )
		return -1;
	for( dword i = 0; i < vmd_info->motion_num; i ++ )
	{
		buf->get_bin( &vmd_info->motion[ i ], sizeof( MMDPI_VMD_MOTION ) );
	}

	// スキン読み込み
	buf->get_bin( &this->vmd_info->skin_num, sizeof( MMDPI_VMD_SKIN_COUNT ) );
	vmd_info->skin = new MMDPI_VMD_SKIN[ this->vmd_info->skin_num.count ];
	if( this->vmd_info->skin == 0x00 )
		return -1;
	for( dword i = 0; i < this->vmd_info->skin_num.count; i ++ )
	{
		buf->get_bin( &this->vmd_info->skin[ i ], sizeof( MMDPI_VMD_SKIN ) );
	}

	delete buf;

	// 解析
	return this->analyze();
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
			ml = 0x00;
			ml = mml;
		}
		skin_line = 0x00;
		delete skin_line;
	}

	delete[] now_motion; now_motion = 0x00;
	delete[] m_list; m_list = 0x00;
}
