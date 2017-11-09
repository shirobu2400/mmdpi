
#include "pmd_load.h"


// データ整列
int	mmdpiPmdLoad::reader( GetBin* buf )
{
	// head
	if( get_header( buf ) )
		return -1;

	// vertex
	buf->get_bin( &vertex_num, sizeof( vertex_num ) );
	vertex = new MMDPI_PMD_VERTEX[ vertex_num ];
	for( dword i = 0; i < vertex_num; i ++ )
	{
		buf->get_bin( &vertex[ i ], sizeof( vertex[ i ] ) );
	}

	// face
	buf->get_bin( &face_num, sizeof( face_num ) );
	//face_num = face_num / 3;
	face = new dword[ face_num ];
	for( dword i = 0; i < face_num; i ++ )
	{
		buf->get_bin2( &face[ i ], sizeof( dword ), sizeof( ushort ) );
	}

	// Matrial
	buf->get_bin( &material_num, sizeof( material_num ) );
	material = new MMDPI_PMD_MATERIAL[ material_num ];
	for( dword i = 0; i < material_num; i ++ )
	{
		buf->get_bin( &material[ i ], sizeof( material[ i ] ) );
		buf->change_enmark( material[ i ].texture_file_name );
	}

	// Bone
	buf->get_bin( &bone_num, sizeof( bone_num ) );
	bone = new MMDPI_PMD_BONE_INFO[ bone_num ];
	for( ushort i = 0; i < bone_num; i ++ )
	{
		buf->get_bin( &bone[ i ], sizeof( bone[ i ] ) );
	}

	// ik
	buf->get_bin( &ik_num, sizeof( ik_num ) );
	ik = new MMDPI_PMD_IK_INFO[ ik_num ];
	for( ushort i = 0; i < ik_num; i ++ )
	{
		buf->get_bin( &ik[ i ].ik_bone_index, sizeof( ik[ i ].ik_bone_index ) );
		buf->get_bin( &ik[ i ].ik_target_bone_index, sizeof( ik[ i ].ik_target_bone_index ) );
		buf->get_bin( &ik[ i ].ik_chain_length, sizeof( ik[ i ].ik_chain_length ) );
		buf->get_bin( &ik[ i ].iterations, sizeof( ik[ i ].iterations ) );
		buf->get_bin( &ik[ i ].control_weight, sizeof( ik[ i ].control_weight ) );

		ik[ i ].ik_child_bone_index = new ushort[ ik[ i ].ik_chain_length ];
		buf->get_bin( ik[ i ].ik_child_bone_index, sizeof( ik[ i ].ik_child_bone_index[ 0 ] ) * ik[ i ].ik_chain_length );
	}

	// skin
	buf->get_bin( &skin_num, sizeof( skin_num ) );
	if( skin_num > 0 )
	{
		skin = new MMDPI_PMD_SKIN[ skin_num ];
		for( ushort i = 0; i < skin_num; i ++ )
		{
			buf->get_bin( skin[ i ].skin_name, sizeof( char ) * 20 );
			buf->get_bin( &skin[ i ].skin_vert_count, sizeof( dword ) );
			buf->get_bin( &skin[ i ].skin_type, sizeof( BYTE ) );

			skin[ i ].vertex = new MMDPI_PMD_SKIN_VERTEX[ skin[ i ].skin_vert_count ];
			buf->get_bin( skin[ i ].vertex, sizeof( MMDPI_PMD_SKIN_VERTEX ) * skin[ i ].skin_vert_count );
		}
	}

	// display skin
	buf->get_bin( &skin_cnt_num, sizeof( BYTE ) );
	if( skin_cnt_num > 0 )
	{
		skin_cnt = new MMDPI_PMD_SKIN_NUMBER[ skin_cnt_num ];

		for( ushort i = 0; i < skin_cnt_num; i ++ )
			buf->get_bin( &skin_cnt[ i ].skin_index, sizeof( ushort ) );
	}

	// ボーン枠用枠名リスト
	buf->get_bin( &bone_name_num, sizeof( BYTE ) );
	bone_name = new MMDPI_PMD_BONE_NAME[ bone_name_num ];
	for( BYTE i = 0; i < bone_name_num; i ++ )
		buf->get_bin( &bone_name[ i ].disp_name, sizeof( char ) * 50 );

	// ボーン枠用表示リスト
	buf->get_bin( &bone_number_num, sizeof( dword ) );
	bone_number = new MMDPI_PMD_BONE_NUMBER[ bone_number_num ];
	for( dword i = 0; i < bone_number_num; i ++ )
	{
		buf->get_bin( &bone_number[ i ].bone_index, sizeof( ushort ) );
		buf->get_bin( &bone_number[ i ].bone_disp_frame_index, sizeof( BYTE ) );
	}

	// 英語情報読み込み

	// 英語対応？
	//fread( &en_h.english_name_compatibility, sizeof( BYTE ), 1, fp );
	if( buf->get_bin( &en_head.english_name_compatibility, sizeof( BYTE ) ) && en_head.english_name_compatibility )
	{
		// 英語ヘッダ
		buf->get_bin( en_head.model_name_eg, sizeof( char ) * 20 );
		buf->get_bin( en_head.comment_eg, sizeof( char ) * 256 );

		//	英語ボーン名
		if( bone_num > 0 )
		{
			en_bone_name = new MMDPI_PMD_ENGLISH_BONE_NAME[ bone_num ];
			buf->get_bin( en_bone_name, sizeof( MMDPI_PMD_ENGLISH_BONE_NAME ) * bone_num );
		}

		//	英語表情名
		if( skin_num > 1 )
		{
			en_skin_name = new MMDPI_PMD_ENGLISH_SKIN_NAME[ skin_num - 1 ];
			buf->get_bin( en_skin_name, sizeof( MMDPI_PMD_ENGLISH_SKIN_NAME ) * ( skin_num - 1 ) );
		}

		//	英語ボーン枠用表示リスト
		if( bone_name_num > 1 )
		{
			en_bone_disp_name = new MMDPI_PMD_ENGLISH_BONE_DISP_NAME[ bone_name_num ];
			buf->get_bin( en_bone_disp_name, sizeof( MMDPI_PMD_ENGLISH_BONE_DISP_NAME ) * ( bone_name_num ) );	
		}
	}

	// toon
	toon_name = new MMDPI_PMD_TOON_NAME[ 1 ];
	//if( buf->get_bin( toon_name, sizeof( MMDPI_PMD_TOON_NAME ) ) )
	//	;
	buf->get_bin( toon_name, sizeof( MMDPI_PMD_TOON_NAME ) );

	p_rigid_num = 0;
	p_joint_num = 0;
	if( buf->get_bin( &p_rigid_num, sizeof( p_rigid_num ) ) && 0 < p_rigid_num && p_rigid_num < ( 1 << 16 ) )
	{
		// 剛体情報
		MMDPI_PMD_PHYSICAL_OBJECT	rigid;
		p_rigid = new MMDPI_PHYSICAL_RIGID_INFO[ p_rigid_num ];
		for( dword i = 0; i < p_rigid_num; i ++ )
		{
			MMDPI_PHYSICAL_RIGID_INFO*	r = &p_rigid[ i ];

			buf->get_bin( &rigid, sizeof( MMDPI_PMD_PHYSICAL_OBJECT ) );

			r->name = new char[ 20 ];
			strcpy( r->name, rigid.rigidbody_name );
			r->eng_name = 0x00;

			r->bone_index = rigid.rigidbody_rel_bone_index;
			r->group = rigid.rigidbody_group_index;
			r->not_touch_group_flag = rigid.rigidbody_group_mask;

			r->size[ 0 ] = rigid.shape_w;
			r->size[ 1 ] = rigid.shape_h;
			r->size[ 2 ] = rigid.shape_d;

			r->type = rigid.shape_type;
			
			if( r->bone_index < bone_num )
			{
				for( int j = 0; j < 3; j ++ )
					r->pos[ j ] = rigid.pos_pos[ j ] + bone[ r->bone_index ].bone_head_pos[ j ];
			}
			for( int j = 0; j < 3; j ++ )
				r->rot[ j ] = rigid.pos_rot[ j ];

			r->mass = rigid.rigidbody_weight;
			r->ac_t = rigid.rigidbody_pos_dim;
			r->ac_r = rigid.rigidbody_rot_dim;

			r->repulsion = rigid.rigidbody_recoil;
			r->friction = rigid.rigidbody_friction;

			r->rigidbody_type = rigid.rigidbody_type;

			//if
		}

		// ジョイント情報
		MMDPI_PMD_PHYSICAL_JOINT	joint;
		buf->get_bin( &p_joint_num, sizeof( p_joint_num ) );
		p_joint = new MMDPI_PHYSICAL_JOINT_INFO[ p_joint_num ];
		for( dword i = 0; i < p_joint_num; i ++ )
		{
			MMDPI_PHYSICAL_JOINT_INFO*	j = &p_joint[ i ];
			buf->get_bin( &joint, sizeof( MMDPI_PMD_PHYSICAL_JOINT ) );
	
			j->name = new char[ 20 ];
			strcpy( j->name, joint.joint_name );

			j->a_index = joint.joint_rigidbody_a;
			j->b_index = joint.joint_rigidbody_b;

			j->eng_name = 0x00;
			for( int k = 0; k < 3; k ++ )
				j->pos[ k ] = joint.joint_pos[ k ];
			for( int k = 0; k < 3; k ++ )
				j->rot[ k ] = joint.joint_rot[ k ];
		
			for( int k = 0; k < 3; k ++ )
				j->rotate_limit1[ k ] = joint.constrain_rot_1[ k ];
			for( int k = 0; k < 3; k ++ )
				j->rotate_limit2[ k ] = joint.constrain_rot_2[ k ];

			for( int k = 0; k < 3; k ++ )
				j->trans_limit1[ k ] = joint.constrain_pos_1[ k ];
			for( int k = 0; k < 3; k ++ )
				j->trans_limit2[ k ] = joint.constrain_pos_2[ k ];

			//	弧度法=>度数法
			for( int k = 0; k < 3; k ++ )
				j->spring_rotate[ k ] = joint.spring_rot[ k ] * 180.0f / ( float )M_PI;
			for( int k = 0; k < 3; k ++ )
				j->spring_trans[ k ] = joint.spring_pos[ k ];
						
			j->type = 0;
		}
	}
		
	return 0;
} 

// ヘッダ処理
int mmdpiPmdLoad::get_header( GetBin* buf )
{
	char magic[ 8 ];	//ファイル識別	"Pmd"		

	bin_string( buf, magic, 3 );
	if( strncmp( magic, "Pmd", 3 ) != 0 ) 
		return -1;	//ヘッダ違い	エラー

	buf->get_bin( &head.version, sizeof( head.version ) );
	
	padding( buf );
	bin_string( buf, head.modelName, 20 );		//モデルネーム
	padding( buf );
	bin_string( buf, head.comment, 256 );		//コメント
	padding( buf );

	return 0;
}

	//バイナリ用文字列抜き出し
int mmdpiPmdLoad::bin_string( GetBin* buf, char *str, int bufRange )
{
	int		i;
	BYTE	c;

	for( i = 0; i < bufRange; i ++ )	//エラー処理をしていない -> 危険
	{
		buf->get_bin( &c, sizeof( c ) );

		if( !c )
			break;

		if( c == 0xfd )
			continue; //パディング

		*str ++ = c;
	}
	// if( !i ) fseek( fp, -( long )sizeof( c ), SEEK_CUR );

	*str = '\0';

	return i;
}

int mmdpiPmdLoad::padding( GetBin* buf )
{
	BYTE	c;

	//パディング
	do
	{
		buf->get_bin( &c, sizeof( c ) );
	} while( c == 0xfd );
	buf->buf --;

	return 0;
}

int mmdpiPmdLoad::get_direcotory( const char *file_name )
{
	int i;
	int len = strlen( file_name );

	directory[ 0 ] = '\0';
	for( i = len; i >= 0 && ( file_name[ i ] != '/' && file_name[ i ] != '\\' ); i -- ) 
		;

	if( i > 0 )
	{
		strncpy( directory, file_name, i );
		directory[ i ++ ] = '/';
		directory[ i ] = '\0';
	}

	return 0;
}

// ロード
int mmdpiPmdLoad::load( const char *file_name )
{
	// ディレクトリ取得
	get_direcotory( file_name );

	//
	GetBin* buf = new GetBin();
	if( buf == 0x00 || buf->load( file_name ) )
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

mmdpiPmdLoad::mmdpiPmdLoad()
{
// Pointer Veriables

	//vertex;
	//face;
	//material;
	//bone;
	//ik;
	//skin;
	//ver_skin;
	//skin_cnt;
	//bone_name;
	//en_bone_name;
	//en_skin_name;
	//en_bone_disp_name;
	//toon_name;
	//phy_obj;
	//phy_joint;

	vertex				= 0x00;
	face				= 0x00;
	material			= 0x00;
	bone				= 0x00;
	ik				= 0x00;
	skin				= 0x00;
	ver_skin			= 0x00;
	skin_cnt			= 0x00;
	bone_name			= 0x00;
	en_bone_name			= 0x00;
	en_skin_name			= 0x00;
	en_bone_disp_name		= 0x00;
	toon_name			= 0x00;
	p_rigid				= 0x00;
	p_joint				= 0x00;
}

mmdpiPmdLoad::~mmdpiPmdLoad()
{
	delete[] vertex;
	delete[] face;
	delete[] material;
	delete[] this->bone;
	if( ik )
	{
		for( ushort i = 0; i < ik_num; i ++ )
			delete[] ik[ i ].ik_child_bone_index;
		delete[] ik;
	}
	if( skin )
	{
		for( ushort i = 0; i < skin_num; i ++ )
			delete[] skin[ i ].vertex;
		delete[] skin;
	}
	delete[] ver_skin;
	delete[] skin_cnt;
	delete[] bone_name;
	delete[] en_bone_name;
	delete[] en_skin_name;
	delete[] en_bone_disp_name;
	delete[] toon_name;
	if( p_rigid )
	{
		for( dword i = 0; i < p_rigid_num; i ++ )
			delete[] p_rigid[ i ].name;
		delete[] p_rigid;
	}
	if( p_joint )
	{
		for( dword i = 0; i < p_joint_num; i ++ )
			delete[] p_joint[ i ].name;
		delete[] p_joint;
	}
}
