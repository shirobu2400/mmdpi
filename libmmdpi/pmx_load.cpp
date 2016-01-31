
#include "pmx_load.h"


// データ整列
int	mmdpiPmxLoad::reader( GetBin* buf )
{
	// head
	if( get_header( buf ) )
		return -1;

	int		vertex_index_size = head.byte[ 2 ];
	// vertex
	buf->get_bin( &vertex_num, sizeof( vertex_num ) );
	vertex = new MMDPI_PMX_VERTEX[ vertex_num ];
	int		bone_index_size = head.byte[ 5 ];
	MMDPI_PMX_VERTEX_PTR	nv;
	
	for( dword i = 0; i < vertex_num; i ++ )
	{
		nv = &vertex[ i ];
		buf->get_bin( nv->pos, sizeof( float ) * 3 );
		buf->get_bin( nv->nor, sizeof( float ) * 3 );
		buf->get_bin( nv->uv, sizeof( float ) * 2 );

		if( head.byte[ 1 ] == 0 )
			nv->add_uv = 0;
		else
		{
			nv->add_uv = new float[ 4 * head.byte[ 1 ] ];
			buf->get_bin( nv->add_uv, sizeof( float ) * 4 * head.byte[ 1 ] );
		}

		buf->get_bin( &nv->weight_calc, sizeof( BYTE ) );

		int		vi;
		for( vi = 0; vi < 4; vi ++ )
			nv->bone_index[ vi ] = 0;

		for( vi = 0; vi < 4; vi ++ )
			nv->bone_value[ vi ] = 0;

		switch( nv->weight_calc )
		{
		case 0:	//	BDEF1
			{
				//BDEF1 ->
				//  n : ボーンIndexサイズ  | ウェイト1.0の単一ボーン(参照Index)

				buf->get_bin2( &nv->bone_index[ 0 ], sizeof( dword ), bone_index_size );
				nv->bone_value[ 0 ] = 1;
			}
			break;
		case 1:	//	BDEF2
			{
				//BDEF2 ->
				//  n : ボーンIndexサイズ  | ボーン1の参照Index
				//  n : ボーンIndexサイズ  | ボーン2の参照Index
				//  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト

				for( vi = 0; vi < 2; vi ++ )
					buf->get_bin2( &nv->bone_index[ vi ], sizeof( dword ), bone_index_size );

				buf->get_bin( &nv->bone_value[ 0 ], sizeof( float ) );
				nv->bone_value[ 1 ] = 1 - nv->bone_value[ 0 ];
			}
			break;
		case 2:	//	BDEF4
			{
				//BDEF4 ->
				//  n : ボーンIndexサイズ  | ボーン1の参照Index
				//  n : ボーンIndexサイズ  | ボーン2の参照Index
				//  n : ボーンIndexサイズ  | ボーン3の参照Index
				//  n : ボーンIndexサイズ  | ボーン4の参照Index
				//  4 : float              | ボーン1のウェイト値
				//  4 : float              | ボーン2のウェイト値
				//  4 : float              | ボーン3のウェイト値
				//  4 : float              | ボーン4のウェイト値 (ウェイト計1.0の保障はない)

				for( vi = 0; vi < 4; vi ++ )
					buf->get_bin2( &nv->bone_index[ vi ], sizeof( dword ), bone_index_size );

				for( vi = 0; vi < 4; vi ++ )
					buf->get_bin( &nv->bone_value[ vi ], sizeof( float ) );
			}
			break;
		case 3:	//	SDEF
			{
				//SDEF ->
				//  n : ボーンIndexサイズ  | ボーン1の参照Index
				//  n : ボーンIndexサイズ  | ボーン2の参照Index
				//  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト
				// 12 : float3             | SDEF-C値(x,y,z)
				// 12 : float3             | SDEF-R0値(x,y,z)
				// 12 : float3             | SDEF-R1値(x,y,z) ※修正値を要計算
				for( vi = 0; vi < 2; vi ++ )
					buf->get_bin2( &nv->bone_index[ vi ], sizeof( dword ), bone_index_size );

				buf->get_bin( &nv->bone_value[ 0 ], sizeof( float ) );
				nv->bone_value[ 1 ] = 1 - nv->bone_value[ 0 ];

				buf->get_bin( nv->sdef_options, sizeof( float ) * ( 3 * 3 ) );
			}
			break;
		}

		buf->get_bin( &nv->edge_scale, sizeof( float ) );
	}
	
	// face
	buf->get_bin( &face_num, sizeof( face_num ) );
	//face = new MMDPI_PMX_FACE[ face_num ];
	face = new dword[ face_num ];
	for( dword i = 0; i < face_num; i ++ )
	{
		buf->get_bin2( &face[ i ], sizeof( face[ i ] ), head.byte[ 2 ] );
	}

	//	Texture
	buf->get_bin( &texture_num, sizeof( dword ) );
	texture = new MMDPI_PMX_TEXTURE[ texture_num ];
	for( dword i = 0; i < texture_num; i ++ )
	{
		uint		buf_len;
		texture[ i ].name = text_buf( buf, &buf_len );
		texture[ i ].sjis_name = convert_sjis( texture[ i ].name, buf_len );
	}

	//	Material
	buf->get_bin( &material_num, sizeof( dword ) );
	material = new MMDPI_PMX_MATERIAL[ material_num ];
	for( dword i = 0; i < material_num; i ++ )
	{
		//	Name
		uint	buf_len;
		material[ i ].name = text_buf( buf, &buf_len );

		//	English name
		material[ i ].eng_name = text_buf( buf );

		//	
		buf->get_bin( material[ i ].Diffuse, sizeof( float ) * 4 );
		buf->get_bin( material[ i ].Specular, sizeof( float ) * 3 );
		buf->get_bin( &material[ i ].Specular_scale, sizeof( float ) );
		buf->get_bin( material[ i ].Ambient, sizeof( float ) * 3 );
		
		buf->get_bin( &material[ i ].bitFlag, sizeof( BYTE ) );
		buf->get_bin( material[ i ].edge_color, sizeof( float ) * 4 );
		buf->get_bin( &material[ i ].edge_size, sizeof( float ) );

		buf->get_bin2( &material[ i ].texture_index, sizeof( dword ), head.byte[ 3 ] );
		buf->get_bin2( &material[ i ].sphere_texture_index, sizeof( dword ), head.byte[ 3 ] );

		//	スフィアモード 0:無効 1:乗算(sph) 2:加算(spa) 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)
		buf->get_bin( &material[ i ].sphere_mode, sizeof( BYTE ) );

		//	共有Toonフラグ 0:継続値は個別Toon 1:継続値は共有Toon
		buf->get_bin( &material[ i ].toon_flag, sizeof( BYTE ) );

		material[ i ].toon_name = new char[ 0xff ];
		if( material[ i ].toon_flag )
		{
			buf->get_bin2( &material[ i ].toon_texture_number, sizeof( dword ), sizeof( BYTE ) );
			sprintf( material[ i ].toon_name, "toon%02lu.bmp", material[ i ].toon_texture_number + 1 );
			//material[ i ].toon_name = convert_sjis( material[ i ].toon_name, strlen( material[ i ].toon_name ) );
		}
		else
		{
			buf->get_bin2( &material[ i ].toon_texture_number, sizeof( dword ), head.byte[ 3 ] );
			if( material[ i ].toon_texture_number < texture_num )
			{
#ifdef _MMDPI_LINUX_OS_
				sprintf( material[ i ].toon_name, "%s", texture[ material[ i ].toon_texture_number ].name );
#else
				sprintf( material[ i ].toon_name, "%s", texture[ material[ i ].toon_texture_number ].sjis_name );
#endif
			}
			else
			{
				delete[] material[ i ].toon_name;
				material[ i ].toon_name = NULL;
			}
		}

		material[ i ].comment = text_buf( buf );

		//	vertex num	4byte
		buf->get_bin( &material[ i ].fver_num, sizeof( dword ) );

		material[ i ].anti_clear_rate = 1;	//	非透明にする
	}

	// Bone
	buf->get_bin( &bone_num, sizeof( bone_num ) );
	bone = new MMDPI_PMX_BONE_INFO[ bone_num ];

	ushort						bone_flag;
	MMDPI_PMX_BONE_INFO_PTR		tbone;
	for( dword i = 0; i < bone_num; i ++ )
	{
		tbone = &bone[ i ];

		//	Name
		uint		buf_len;
		tbone->name = text_buf( buf, &buf_len );
		tbone->sjis_name = convert_sjis( tbone->name, buf_len );

		//	English name
		tbone->eng_name = text_buf( buf );

		//	pos
		buf->get_bin( tbone->pos, sizeof( float ) * 3 );

		//	parent
		buf->get_bin2( &tbone->parent_index, sizeof( dword ), head.byte[ 5 ] );

		//	level
		buf->get_bin( &tbone->level, sizeof( dword ) );

		//	flag
		buf->get_bin( &tbone->bone_flag, sizeof( ushort ) );

		//	switch
		bone_flag = tbone->bone_flag;
		
		if( bone_flag & 0x0001 )	//	接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
		{
			buf->get_bin2( &tbone->child_index, sizeof( dword ), head.byte[ 5 ] );
			tbone->child_flag = 1;
		}
		else
		{
			buf->get_bin( tbone->offset, sizeof( float ) * 3 );
			tbone->child_flag = 0;
		}

		if( bone_flag & 0x0002 )	//	回転可能
		{
			tbone->rotation_flag = 1;
		}

		if( bone_flag & 0x0004 )	//	移動可能
		{
			tbone->translate_flag = 1;
		}

		if( bone_flag & 0x0008 )	//	表示 
			tbone->show_flag = 1;

		if( bone_flag & 0x0010 )	//	操作可能
			tbone->user_update_flag = 1;

		if( bone_flag & 0x0100 )	//	回転付与
		{
			buf->get_bin2( &tbone->tr_parent_index, sizeof( dword ), head.byte[ 5 ] );
			buf->get_bin( &tbone->tr_parent_rate, sizeof( float ) );

			tbone->rotation_s_flag = 1;
		}

		if( bone_flag & 0x0200 )	//	移動付与
		{
			buf->get_bin2( &tbone->tr_parent_index, sizeof( dword ), head.byte[ 5 ] );
			buf->get_bin( &tbone->tr_parent_rate, sizeof( float ) );

			tbone->translate_s_flag = 1;
		}

		if( bone_flag & 0x0400 )	//	軸固定
		{
			buf->get_bin( tbone->axis_vector, sizeof( float ) * 3 );
			tbone->const_axis_flag = 1;
		}

		if( bone_flag & 0x0800 )	//	ローカル軸
		{
			buf->get_bin( tbone->local_axis_x, sizeof( float ) * 3 );
			buf->get_bin( tbone->local_axis_z, sizeof( float ) * 3 );

			tbone->local_axis_flag = 1;
		}

		if( bone_flag & 0x1000 )	//	物理後変形軸
		{
			tbone->physical_update_flag = 1;
		}

		if( bone_flag & 0x2000 )	//	外部親変形
		{
			buf->get_bin( &tbone->key_value, sizeof( dword ) );
			tbone->out_parent_update_flag = 1;
		}


		if( bone_flag & 0x0020 )	//	IK
		{
			buf->get_bin2( &tbone->ik_target_bone_index, sizeof( dword ), head.byte[ 5 ] );
			buf->get_bin( &tbone->ik_loop_num, sizeof( dword ) );

			buf->get_bin( &tbone->ik_radius_range, sizeof( float ) );
			buf->get_bin( &tbone->ik_link_num, sizeof( dword ) );
			tbone->ik_link = 0x00;
			if( tbone->ik_link_num > 0 )
				tbone->ik_link = new MMDPI_PMX_IK_INFO[ tbone->ik_link_num ];
			for( dword j = 0; j < tbone->ik_link_num; j ++ )
			{
				buf->get_bin2( &tbone->ik_link[ j ].ik_bone_index, sizeof( dword ), head.byte[ 5 ] );
				buf->get_bin( &tbone->ik_link[ j ].rotate_limit_flag, sizeof( BYTE ) );

				if( tbone->ik_link[ j ].rotate_limit_flag )
				{
					buf->get_bin( tbone->ik_link[ j ].bottom, sizeof( float ) * 3 );
					buf->get_bin( tbone->ik_link[ j ].top, sizeof( float ) * 3 );

					//tbone->ik_link[ j ].bottom[ 0 ] = -tbone->ik_link[ j ].bottom[ 0 ];
					//tbone->ik_link[ j ].top[ 0 ] = -tbone->ik_link[ j ].top[ 0 ];
				}
			}
			tbone->ik_flag = 1;
		}
		else
			bone[ i ].ik_link = NULL;
	}

	//	モーフ
	buf->get_bin( &morph_num, sizeof( dword ) );
	morph = new MMDPI_PMX_MORPH_INFO[ morph_num ];
	for( dword i = 0; i < morph_num; i ++ )
	{
		MMDPI_PMX_MORPH_INFO_PTR	m = &morph[ i ];
		
		m->name = text_buf( buf );
		m->eng_name = text_buf( buf );

		m->sjis_name = convert_sjis( m->name, strlen( m->name ) );

		buf->get_bin( &m->panel, sizeof( BYTE ) );
		buf->get_bin( &m->type, sizeof( BYTE ) );
		
		buf->get_bin( &m->offset_num, sizeof( dword ) );

		uint	uv_number = 0;
		switch( m->type )
		{
		case 1:
			m->vertex = new MMDPI_PMX_MORPH_INFO_VERTEX[ m->offset_num ];
			for( uint j = 0; j < m->offset_num; j ++ )
			{
				buf->get_bin2( &m->vertex[ j ].vertex_id, sizeof( dword ), head.byte[ 2 ] );
				buf->get_bin( m->vertex[ j ].vertex, sizeof( float ) * 3 );
			}
			break;
			
		//	追加UV
		case 4:	uv_number = 0;
		case 5:	uv_number = 1;
		case 6:	uv_number = 2;
		case 7:	uv_number = 3;

		case 3:
			m->uv = new MMDPI_PMX_MORPH_INFO_UV[ m->offset_num ];
			for( uint j = 0; j < m->offset_num; j ++ )
			{
				m->uv[ j ].uv_number = uv_number;
				buf->get_bin2( &m->uv[ j ].uv_id, sizeof( dword ), head.byte[ 2 ] );
				buf->get_bin( m->uv[ j ].uv, sizeof( float ) * 4 );
			}
			break;

		case 2:
			m->bone = new MMDPI_PMX_MORPH_INFO_BONE[ m->offset_num ];
			for( uint j = 0; j < m->offset_num; j ++ )
			{
				buf->get_bin2( &m->bone[ j ].bone_id, sizeof( dword ), head.byte[ 5 ] );
				buf->get_bin( m->bone[ j ].translate, sizeof( float ) * 3 );
				buf->get_bin( m->bone[ j ].rotation, sizeof( float ) * 4 );
			}
			break;

		case 8:		//	材質モーフ
			m->material = new MMDPI_PMX_MORPH_INFO_MATERIAL[ m->offset_num ];
			for( uint j = 0; j < m->offset_num; j ++ )
			{
				buf->get_bin2( &m->material[ j ].material_id, sizeof( dword ), head.byte[ 4 ] );
				buf->get_bin( &m->material[ j ].calc_format, sizeof( BYTE ) );
				buf->get_bin( m->material[ j ].diffuse, sizeof( float ) * 4 );
				buf->get_bin( m->material[ j ].specular, sizeof( float ) * 3 );
				buf->get_bin( &m->material[ j ].specular_alpha, sizeof( float ) );
				buf->get_bin( m->material[ j ].ambient, sizeof( float ) * 3 );
				buf->get_bin( m->material[ j ].edge_color, sizeof( float ) * 4 );
				buf->get_bin( &m->material[ j ].edge_size, sizeof( float ) );
				buf->get_bin( m->material[ j ].texture_alpha, sizeof( float ) * 4 );
				buf->get_bin( m->material[ j ].sphere_alpha, sizeof( float ) * 4 );
				buf->get_bin( m->material[ j ].toon_texture_alpha, sizeof( float ) * 4 );
			}
			break;

		case 0:
			m->group = new MMDPI_PMX_MORPH_INFO_GROUP[ m->offset_num ];
			for( uint j = 0; j < m->offset_num; j ++ )
			{
				buf->get_bin2( &m->group[ j ].group_id, sizeof( dword ), head.byte[ 6 ] );
				buf->get_bin( &m->group[ j ].morph_rate, sizeof( float ) );
			}
			break;
		}
	}

	//	表示枠
	buf->get_bin( &show_num, sizeof( dword ) );
	show = new MMDPI_PMX_SHOW_FRAME_INFO[ show_num ];
	for( uint i = 0; i < show_num; i ++ )
	{
		MMDPI_PMX_SHOW_FRAME_INFO_PTR	s = &show[ i ];
		
		s->name = text_buf( buf );
		s->eng_name = text_buf( buf );
		
		buf->get_bin( &s->frame_flag, sizeof( BYTE ) );
		buf->get_bin2( &s->index_num, sizeof( dword ), 4 );
		
		s->target_index = new MMDPI_PMX_SHOW_FRAME_INFO_INLINE[ s->index_num ];
		for( uint j = 0; j < s->index_num; j ++ )
		{
			buf->get_bin( &s->target_index[ j ].type, sizeof( BYTE ) );

			int		index_size = head.byte[ 5 ];
			if( s->target_index[ j ].type )	//	モーフ
				index_size = head.byte[ 6 ];
			buf->get_bin2( &s->target_index[ j ].index, sizeof( dword ), index_size );
		}
	}

	p_rigid_num = 0;
	if( buf->get_bin( &p_rigid_num, sizeof( dword ) ) == NULL )
	{
		p_rigid_num = 0;
		return 0;
	}
	if( !( 0 < p_rigid_num && p_rigid_num < bone_num ) )
	{
		p_rigid_num = 0;
		return 0;
	}
	p_rigid = new MMDPI_PHYSICAL_RIGID_INFO[ p_rigid_num ];

	// 剛体情報
	for( uint i = 0; i < p_rigid_num; i ++ )
	{
		MMDPI_PHYSICAL_RIGID_INFO_PTR	rigid = &p_rigid[ i ];
		
		uint		name_length;
		rigid->name = text_buf( buf, &name_length );
		rigid->name = convert_sjis( rigid->name, name_length );
		rigid->eng_name = text_buf( buf );
		
		buf->get_bin2( &rigid->bone_index, sizeof( dword ), head.byte[ 5 ] );
		buf->get_bin( &rigid->group, sizeof( BYTE ) );
		buf->get_bin( &rigid->not_touch_group_flag, sizeof( ushort ) );

		buf->get_bin( &rigid->type, sizeof( BYTE ) );

		buf->get_bin( rigid->size, sizeof( float ) * 3 );

		buf->get_bin( rigid->pos, sizeof( float ) * 3 );
		buf->get_bin( rigid->rot, sizeof( float ) * 3 );

		buf->get_bin( &rigid->mass, sizeof( float ) );
		buf->get_bin( &rigid->ac_t, sizeof( float ) );
		buf->get_bin( &rigid->ac_r, sizeof( float ) );

		buf->get_bin( &rigid->repulsion, sizeof( float ) );
		buf->get_bin( &rigid->friction, sizeof( float ) );

		buf->get_bin( &rigid->rigidbody_type, sizeof( BYTE ) );
	}

	// ジョイント情報
	buf->get_bin( &p_joint_num, sizeof( dword ) );
	p_joint = new MMDPI_PHYSICAL_JOINT_INFO[ p_joint_num ];
	for( uint i = 0; i < p_joint_num; i ++ )
	{
		MMDPI_PHYSICAL_JOINT_INFO_PTR	joint = &p_joint[ i ];

		uint		name_length;
		joint->name = text_buf( buf, &name_length );
		joint->name = convert_sjis( joint->name, name_length );
		joint->eng_name = text_buf( buf );
		
		buf->get_bin( &joint->type, sizeof( BYTE ) );

		if( joint->type )
			continue;

		buf->get_bin2( &joint->a_index, sizeof( dword ), head.byte[ 7 ] );
		buf->get_bin2( &joint->b_index, sizeof( dword ), head.byte[ 7 ] );

		buf->get_bin( joint->pos, sizeof( float ) * 3 );
		buf->get_bin( joint->rot, sizeof( float ) * 3 );

		buf->get_bin( joint->trans_limit1, sizeof( float ) * 3 );
		buf->get_bin( joint->trans_limit2, sizeof( float ) * 3 );

		buf->get_bin( joint->rotate_limit1, sizeof( float ) * 3 );
		buf->get_bin( joint->rotate_limit2, sizeof( float ) * 3 );
	
		buf->get_bin( joint->spring_trans, sizeof( float ) * 3 );
		buf->get_bin( joint->spring_rotate, sizeof( float ) * 3 );
	}

	return 0;
} 

// ヘッダ処理
int mmdpiPmxLoad::get_header( GetBin* buf )
{
	char	magic[ 8 ];	//ファイル識別	"Pmd"		
	
	buf->get_bin( magic, 4 );
	if( strncmp( magic, "Pmx ", 4 ) != 0 && strncmp( magic, "PMX ", 4 ) != 0 ) 
		return -1;	//ヘッダ違い	エラー

	//	バージョン
	buf->get_bin( &head.version, sizeof( head.version ) );

	//	各種バイト情報
	buf->get_bin( &head.byte_num, 1 );
	head.byte = new BYTE[ head.byte_num + 1 ];
	buf->get_bin( head.byte, head.byte_num );

	byte_one_length = ( head.byte[ 0 ] == 1 )? 1 : 2 ;	//	utf8 or utf16

	uint	name_length, name_eng_length, comment_length, comment_eng_length;

	//setlocale( LC_CTYPE, "jpn" );
	//setlocale( LC_CTYPE, "ja_JP.UTF-8" );

	head.name = text_buf( buf, &name_length );			//	モデルネーム

	head.name_eng = text_buf( buf, &name_eng_length );		//	英モデルネーム

	head.comment = text_buf( buf, &comment_length );			//	コメント
	
	head.comment_eng = text_buf( buf, &comment_eng_length );		//	英コメント

#ifndef _MMDPI_LINUX_OS_
	if( head.name )
		head.name = convert_sjis( head.name, name_length, 1 );			//	モデルネーム
	if( head.name_eng )
		head.name_eng = convert_sjis( head.name_eng, name_eng_length, 1 );		//	英モデルネーム
	if( head.comment )
		head.comment = convert_sjis( head.comment, comment_length, 1 );			//	コメント
	if( head.comment_eng )
		head.comment_eng = convert_sjis( head.comment_eng, comment_eng_length, 1 );		//	英コメント
#endif

	if( head.name )
	{
		puts( "Model:" );
		puts( head.name );
	}
	
	if( head.comment )
	{
		puts( "Comment:" );
		puts( head.comment );
	}

	if( head.name_eng )
	{
		puts( "Model English:" );
		puts( head.name_eng );
	}

	if( head.comment_eng )
	{
		puts( "Comment English:" );
		puts( head.comment_eng );
	}

	return 0;
}

char* mmdpiPmxLoad::text_buf( GetBin* buf, uint* length )
{
	char*	text	 = NULL;
	long	byte_len = 0;

	if( length )
		*length = 0;

	buf->get_bin( &byte_len, 4 );
	if( byte_len < 1 )
		return NULL;
	
	text = new char[ byte_len + 4 ];
	if( text == NULL )
	{
		puts( "Text buf cannot Allocation." );
		return NULL;
	}
	memset( text, 0, byte_len + 4 );
	buf->get_bin( text, byte_len );
	text[ byte_len ] = '\0';
		
	if( head.byte[ 0 ] )
		;
	else
	{
		text[ byte_len + 1 ] = '\0';
		text = convert_utf8( text, byte_len / byte_one_length + 1 );
		byte_len = strlen( text );
	}

	buf->change_enmark( text );

	if( length )
		*length = byte_len;
	//
	return text;
}

char* mmdpiPmxLoad::convert_sjis( char* text, uint byte_len, int text_release_flag )
{
	if( byte_len < 1 )
		return NULL;

	char*		text2 = new char[ byte_len + 4 ];
	memset( text2, 0, byte_len + 2 );
	Utf8toSJIS( text2, byte_len, ( const char * )text, byte_len );

	if( text_release_flag )
		delete[] text;

	return text2;
}

int	mmdpiPmxLoad::Utf8toSJIS( char *dest, size_t dest_size, const char *src, size_t src_size )
{
    char				mb[ 2 ] = { 0 };  
	unsigned short		wc = 0;
	
    for( uint i = 0, j = 0; src[ j ] && j < src_size; i ++, j ++ )
	{
		unsigned char	c = ( unsigned char )src[ j ] & 0xff;
		wc = 0x00;
		if( c < 0x80 )
            wc = c & 0xff;
        else if( c < 0xc0 )
            continue;
        else if( c < 0xe0 )
		{
            wc = ( unsigned short )( c & 0x1f ) << 6;
			j ++;
			if( ( c = src[ j ] ) == 0x00 ) 
				break;
            wc |= c & 0x3f;
        }
        else if( c < 0xf0 )
		{
            wc = ( unsigned short )( src[ j ] & 0x0f ) << 12;
			j ++;
            if( ( c = src[ j ] ) == 0x00 )
				break;
			wc |= ( unsigned short )( c & 0x3f ) << 6;
            j ++;
			if( ( c = src[ j ] ) == 0x00 )
				break;
			wc |= c & 0x3f;
        }
        else
            continue;

		//c = wctomb( mb, wc );
		mb[ 1 ] = ( char )( utf8tosjis[ wc ] & 0xff );
		mb[ 0 ] = ( char )( ( utf8tosjis[ wc ] >> 8 ) & 0xff );

		c = ( ( mb[ 0 ] )? 1 : 0 ) + ( ( mb[ 1 ] )? 1 : 0 );
		if( c == 1 ) 
		{
			dest[ i ] = mb[ 1 ];
		}
		else if( c > 1 )
		{
			dest[ i ] = mb[ 0 ];
			i ++;
			dest[ i ] = mb[ 1 ];
		}
    }

	return 0;
}

/**
 * 文字コードをUTF-16よりUTF-8へと変換。
 * 
 * @param[out] dest 出力文字列UTF-8
 * @param[in]  dest_size destのバイト数
 * @param[in]  src 入力文字列UTF-16
 * @param[in]  src_size 入力文字列の文字数
 * 
 * @return 成功時には出力文字列のバイト数を戻します。
 *         dest_size に0を指定し、こちらの関数を呼び出すと、変換された
 *         文字列を格納するのに必要なdestのバイト数を戻します。
 *         関数が失敗した場合には、FALSEを戻します。
 */
int mmdpiPmxLoad::Utf16ToUtf8( char *dest, size_t dest_size, const short *src, size_t src_size )
{
/* ビットパターン
 * ┌───────┬───────┬───────────────────────────┬──────────────────┐
 * │フォーマット  │Unicode       │UTF-8ビット列                                         │Unicodeビット列                     │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │1バイトコード │\u0～\u7f     │0aaabbbb                                              │00000000 0aaabbbb                   │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │2バイトコード │\u80～\u7ff   │110aaabb 10bbcccc                                     │00000aaa bbbbcccc                   │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │3バイトコード │\u800～\uffff │1110aaaa 10bbbbcc 10ccdddd                            │aaaabbbb ccccdddd                   │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │4バイトコード │--------------│11110??? 10?????? 10?????? 10??????                   │未対応                              │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │5バイトコード │--------------│111110?? 10?????? 10?????? 10?????? 10??????          │未対応                              │
 * ├───────┼───────┼───────────────────────────┼──────────────────┤
 * │6バイトコード │--------------│1111110? 10?????? 10?????? 10?????? 10?????? 10?????? │未対応                              │
 * └───────┴───────┴───────────────────────────┴──────────────────┘
 */
    long				countNeedsBytes;
    unsigned long		cursor;
    unsigned short		wcWork1;
    long				sizeBytes;
    char				chWork1;
    char				chWork2;
    char				chWork3;
    
    if( src == NULL )
    {
        /* Error : src is NULL. */
        return 0;
    }
    if( src_size < 0 )
    {
        /* Error : src_size < 0. */
        return 0;
    }
    
    countNeedsBytes = 0;
    for( cursor = 0; cursor < src_size; cursor ++ )
    {
        /* srcより1ワードのデータを読み出し */
       if( *( unsigned char* )src + cursor == ( unsigned char )0x00 )
			break;

        wcWork1 = ( unsigned short )*( ( const unsigned short* )src + cursor );
		if( wcWork1 <= ( unsigned short )0x007f )
        {
            /* 0x0000 to 0x007f */
            sizeBytes = 1;
        }
        else if( ( unsigned short )0x0080 <= wcWork1 && wcWork1 <= ( unsigned short )0x07ff )
        {
            /* 0x0080 to 0x07ff */
            sizeBytes = 2;
        }
        else if( ( unsigned short )0x0800 <= wcWork1 )
        {
            /* 0x0800 to 0xffff */
            sizeBytes = 3;
        }
        else
        {
            /* Error : unknown code */
            return 0;
        }

        /* sizeBytes毎に処理を分岐 */
        if( 0 < dest_size && dest )
		{
			/*
			 * dest_size をチェック
			 */
			if( dest_size < ( unsigned )( countNeedsBytes + sizeBytes ) )
			{
				/* Error : memory is not enough for dest */
				return countNeedsBytes;
			}
        
			switch( sizeBytes )
			{
			case 1:
				/*
				 * ビット列
				 * (0aaabbbb)UTF-8 ... (00000000 0aaabbbb)UTF-16
				 */

				*dest = ( char )wcWork1;              /* 0aaabbbb */
				dest ++;
				break;
			case 2:
				/*
				 * ビット列
				 * (110aaabb 10bbcccc)UTF-8 ... (00000aaa bbbbcccc)UTF-16
				 */
				chWork1 =  ( char )( wcWork1 >> 6 );     /* 000aaabb */
				chWork1 |= ( char )0xc0;              /* 110aaabb */
				chWork2 =  ( char )wcWork1 & 0xff;            /* bbbbcccc */
				chWork2 &= ( char )0x3f;              /* 00bbcccc */
				chWork2 |= ( char )0x80;              /* 10bbcccc */

				*dest = chWork1;
				dest++;
				*dest = chWork2;
				dest++;
				break;
			case 3:
				/*
				 * ビット列
				 * (1110aaaa 10bbbbcc 10ccdddd)UTF-8 ... (aaaabbbb ccccdddd)UTF-16
				 */
				chWork1 =  ( char )( wcWork1 >> 12 );    /* ????aaaa */
				chWork1 &= ( char )0x0f;              /* 0000aaaa */
				chWork1 |= ( char )0xe0;              /* 1110aaaa */
				chWork2 =  ( char )( wcWork1 >> 6 );     /* aabbbbcc */
				chWork2 &= ( char )0x3f;              /* 00bbbbcc */
				chWork2 |= ( char )0x80;              /* 10bbbbcc */
				chWork3 =  ( char )wcWork1 & 0xff;            /* ccccdddd */
				chWork3 &= ( char )0x3f;              /* 00ccdddd */
				chWork3 |= ( char )0x80;              /* 10ccdddd */

				*dest = chWork1;
				dest++;
				*dest = chWork2;
				dest++;
				*dest = chWork3;
				dest++;
				break;
			default:
				break;
			}
		}

        countNeedsBytes += sizeBytes;
    }

    return countNeedsBytes;
}

char mmdpiPmxLoad::utf8mbleb( const char* src )
{
    /* srcより1ワードのデータを読み出し */
	unsigned short	wcWork1 = ( unsigned short )*( ( const unsigned short* )src );
	char			sizeBytes = 1;

	//if( wcWork1 == ( unsigned short )0x0000 )
	//	break;

    if( ( wcWork1 <= ( ( unsigned short )0x007f ) ) )
    {
        /* 0x0000 to 0x007f */
        sizeBytes = 1;
    }
    else if( ( ( ( unsigned short )0x0080 ) <= wcWork1 ) && ( wcWork1 <= ( ( unsigned short )0x07ff ) ) )
    {
        /* 0x0080 to 0x07ff */
        sizeBytes = 2;
    }
    else if ( ( ( ( unsigned short )0x0800 ) <= wcWork1 ) )
    {
        /* 0x0800 to 0xffff */
        sizeBytes = 3;
    }
    else
    {
        /* Error : unknown code */
        return 1;
    }

	return sizeBytes;
}

char* mmdpiPmxLoad::convert_utf8( char* text, uint byte_len )
{
	int		length = ( int )Utf16ToUtf8( NULL, 0, ( const short* )text, byte_len );
	char*	text2 = new char[ length + 2 ];
	if( text2 == NULL )
	{
		printf( "Text buf cannot Allocation. : %d length.\n", length );
		return NULL;
	}

	Utf16ToUtf8( text2, length, ( const short* )text, byte_len );
	delete[] text;

	return text2;
}
	//バイナリ用文字列抜き出し
char* mmdpiPmxLoad::bin_string( GetBin* buf )
{
	char*			result;
	vector<char>	_string;
	char			c[ 8 ];

	while( buf->get_bin( c, 1 ) && c[ 0 ] )
		_string.push_back( c[ 0 ] );
	_string.push_back( '\0' );

	result = new char[ _string.size() + 1 ];
	for( unsigned int i = 0; i < _string.size(); i ++ )
		result[ i ] = _string[ i ];
	
	return result;
}

int mmdpiPmxLoad::get_direcotory( const char *file_name )
{
	int		i;
	int		len = strlen( file_name );
	int		d_line = len - 1;

	directory[ 0 ] = '\0';
	
#ifdef _MMDPI_LINUX_OS_
	setlocale( LC_CTYPE, "ja_JP.UTF-8" );
#else
	setlocale( LC_CTYPE, "jpn" );
#endif
	for( i = 0; i < len; i += 1/*this->utf8mbleb( &file_name[ i ] )*/ )
	{ 
		if( file_name[ i ] == '/' || file_name[ i ] == '\\' )
			d_line = i;
	}
	/*
	for( i = len - 1; i >= 0 && ( file_name[ i ] != '/' && file_name[ i ] != '\\' ); i -- ) 
		;
	d_line = i;
	*/
	if( d_line > 0 )
	{
		strncpy( directory, file_name, d_line );
		directory[ d_line ++ ] = '/';
		directory[ d_line ] = '\0';
	}

	return 0;
}

// ロード
int	mmdpiPmxLoad::load( const char *file_name )
{
	// ディレクトリ取得
	get_direcotory( file_name );

	//
	GetBin* buf = new GetBin();
	if( buf->load( file_name ) )
		return -1;

	// Load Main
	if( reader( buf ) )
	{
		delete buf;
		return -1;
	}

	delete buf;

	return 0;
}

int mmdpiPmxLoad::char_byte( const char* _string )
{
#ifdef _MMDPI_LINUX_OS_
	return ( *_string < 0x7f )? 1 : ( *_string < 0x7ff )? 2 : 3 ;
#else
	return ( *_string < 0xe0 )? 1 : 2 ;
#endif
}

mmdpiPmxLoad::mmdpiPmxLoad()
{
	head.byte = NULL;
// Pointer Veriables
	head.name			= 0x00;
	head.name_eng		= 0x00;
	head.comment		= 0x00;
	head.comment_eng	= 0x00;

	vertex				= NULL;
	face				= NULL;
	texture				= NULL;
	material			= NULL;
	bone				= NULL;
	morph				= NULL;
	show				= NULL;
	p_rigid				= NULL;
	p_joint				= NULL;
}

mmdpiPmxLoad::~mmdpiPmxLoad()
{
	delete[] head.byte;

	delete[] head.name;
	delete[] head.name_eng;
	delete[] head.comment;
	delete[] head.comment_eng;

	if( vertex )
	{
		for( uint i = 0; i < vertex_num; i ++ )
			delete[] vertex[ i ].add_uv;
		delete[] vertex;
	}
	delete[] face;

	if( texture )
	{
		for( dword i = 0; i < texture_num; i ++ )
		{
			delete[] texture[ i ].name;
			delete[] texture[ i ].sjis_name;
		}
		delete[] texture;
	}

	if( material )
	{
		for( dword i = 0; i < material_num; i ++ )
		{
			delete[] material[ i ].name;
			delete[] material[ i ].eng_name;
			delete[] material[ i ].comment;
		}
		delete[] material;
	}

	if( bone )
	{
		for( dword i = 0; i < bone_num; i ++ )
		{
			delete[] bone[ i ].name;
			delete[] bone[ i ].eng_name;
			delete[] bone[ i ].sjis_name;

			delete[] bone[ i ].ik_link;
		}
		delete[] bone;
	}

	if( morph )
	{
		for( dword i = 0; i < morph_num; i ++ )
		{
			delete[] morph[ i ].name;
			delete[] morph[ i ].eng_name;

			delete[] morph[ i ].vertex;
			delete[] morph[ i ].uv;
			delete[] morph[ i ].bone;
			delete[] morph[ i ].material;
			delete[] morph[ i ].group;
		}
		delete[] morph;
	}

	if( show )
	{
		for( dword i = 0; i < show_num; i ++ )
		{
			delete[] show[ i ].name;
			delete[] show[ i ].eng_name;
		}
		delete[] show;
	}

	if( p_rigid )
	{
		for( dword i = 0; i < p_rigid_num; i ++ )
		{
			delete[] p_rigid[ i ].name;
			delete[] p_rigid[ i ].eng_name;
		}
		delete[] p_rigid;
	}


	if( p_joint )
	{
		for( dword i = 0; i < p_joint_num; i ++ )
		{
			delete[] p_joint[ i ].name;
			delete[] p_joint[ i ].eng_name;
		}
		delete[] p_joint;
	}
}
