
#include "pmx_analyze.h"


int mmdpiPmxAnalyze::load( const char *file_name )
{
	if( mmdpiPmxLoad::load( file_name ) )
		return -1;
	if( mmdpiModel::create() )
		return -1;
	return analyze();
}

//	解析
int mmdpiPmxAnalyze::analyze( void )
{
	adjust_material = new MMDPI_MATERIAL[ material_num ];
	dword	face_top = 0;
	for( dword i = 0; i < material_num; i ++ )
	{
		adjust_material[ i ].face_top = face_top;
		adjust_material[ i ].face_num = material[ i ].fver_num;
		face_top += material[ i ].fver_num;
	}

	adjust_vertex = new MMDPI_BLOCK_VERTEX();
	if( adjust_vertex == 0x00 )
		return -1;
	adjust_vertex->alloc( mmdpiPmxLoad::vertex_num );

	for( dword i = 0; i < mmdpiPmxLoad::vertex_num; i ++ )
	{
		MMDPI_PMX_VERTEX_PTR	ver = &vertex[ i ];

		adjust_vertex->ver[ i ].x = ver->pos[ 0 ];
		adjust_vertex->ver[ i ].y = ver->pos[ 1 ];
		adjust_vertex->ver[ i ].z = ver->pos[ 2 ];

		adjust_vertex->uv[ i ].x = ver->uv[ 0 ];
		adjust_vertex->uv[ i ].y = 1 - ver->uv[ 1 ];
		adjust_vertex->uv[ i ].z = 0;
		adjust_vertex->uv[ i ].w = 0;

		adjust_vertex->nor[ i ].x = ver->nor[ 0 ];
		adjust_vertex->nor[ i ].y = ver->nor[ 1 ];
		adjust_vertex->nor[ i ].z = ver->nor[ 2 ];

		//	ボーン
		adjust_vertex->index[ i ].x = ( float )ver->bone_index[ 0 ];
		adjust_vertex->index[ i ].y = ( float )ver->bone_index[ 1 ];
		adjust_vertex->index[ i ].z = ( float )ver->bone_index[ 2 ];
		adjust_vertex->index[ i ].w = ( float )ver->bone_index[ 3 ];
			
		//	重み
		adjust_vertex->weight[ i ].x = ver->bone_value[ 0 ];
		adjust_vertex->weight[ i ].y = ver->bone_value[ 1 ];
		adjust_vertex->weight[ i ].z = ver->bone_value[ 2 ];
		adjust_vertex->weight[ i ].w = ver->bone_value[ 3 ];
	}

	//	最適化
	mmdpiAdjust::adjust( adjust_vertex, mmdpiPmxLoad::vertex_num, face, face_num, adjust_material, material_num, mmdpiBone::bone, mmdpiPmxLoad::bone_num );

	//	テクスチャ
	load_texture();
			
	//	マテリアル情報の保存
	for( dword j = 0; j < mesh.size(); j ++ )
	{		
		MMDPI_PIECE*			m	= mesh[ j ]->b_piece;
		MMDPI_PMX_MATERIAL_PTR		mpmx	= &material[ m->raw_material_id ];
		
		m->edge_size = mpmx->edge_size;

		m->edge_color.r = mpmx->edge_color[ 0 ];
		m->edge_color.g = mpmx->edge_color[ 1 ];
		m->edge_color.b = mpmx->edge_color[ 2 ];
		m->edge_color.a = mpmx->edge_color[ 3 ];

		m->opacity = mpmx->anti_clear_rate;

		m->color.r = mpmx->diffuse[ 0 ];
		m->color.g = mpmx->diffuse[ 1 ];
		m->color.b = mpmx->diffuse[ 2 ];
		m->color.a = mpmx->diffuse[ 3 ];

		//	テクスチャ優先
		if( m->has_texture )
			m->color.a = 0;

		//	表示面の設定
		m->is_draw_both = 0;
		if( mpmx->bit_flag & 0x01 )
			m->is_draw_both = 1;
	}

	return 0;
}

void mmdpiPmxAnalyze::load_texture( void )
{
	//	テクスチャ
	dword	fver_num_base = 0;
	long	ppid = -1;
	dword	pi = 0;
	uint	material_num = mesh.size();

	//	一時的に読み込み
	texture			= new MMDPI_IMAGE[ material_num ];
	toon_texture		= new MMDPI_IMAGE[ material_num ];

	texture00		= new MMDPI_IMAGE[ texture_num ];
	//toon_texture00		= new MMDPI_IMAGE[ 10 ];
	for( dword i = 0; i < texture_num; i ++ )
	{
		char*	texture_file_name;
		char	texture_file_name_full[ 0x1000 ];
		int	j, k;	

		texture_file_name = mmdpiPmxLoad::texture[ i ].name;


		for( k = 0; directory[ k ]; k ++ )
			texture_file_name_full[ k ] = directory[ k ];

		for( j = 0; texture_file_name[ j ] && texture_file_name[ j ] != '*'; j ++, k ++ )
			texture_file_name_full[ k ] = texture_file_name[ j ];
		texture_file_name_full[ k ] = '\0';

		//	読み込み成功
		if( texture00[ i ].load( texture_file_name_full ) >= 0 )
			continue;

		//	utf8, sjis での読み込みを試行するので２回読み込みを実行する

		//	name での読み込み失敗
		texture_file_name = mmdpiPmxLoad::texture[ i ].sjis_name;
		
		for( k = 0; directory[ k ]; k ++ )
			texture_file_name_full[ k ] = directory[ k ];

		for( j = 0; texture_file_name[ j ] && texture_file_name[ j ] != '*'; j ++, k ++ )
			texture_file_name_full[ k ] = texture_file_name[ j ];
		texture_file_name_full[ k ] = '\0';

		//	全ての場合で失敗
		//if( texture00[ i ].load( texture_file_name_full ) < 0 )
		//{
		//	printf( "Texture load error.\n" );
		//	printf( "\t=> %s\n", mmdpiPmxLoad::texture[ i ].name      );
		//	printf( "\t=> %s\n", mmdpiPmxLoad::texture[ i ].sjis_name );
		//}
		texture00[ i ].load( texture_file_name_full );
	}

	////	Toon texture
	//for( dword i = 0; i < 10; i ++ )
	//{
	//	char	texture_file_name_full[ 0x200 ];
	//	sprintf( texture_file_name_full, "toon/toon%02d.bmp", ( int )i + 1 );
	//	toon_texture00[ i ].load( texture_file_name_full );
	//}

	for( dword j = 0; j < mesh.size(); j ++ )
	{
		MMDPI_PIECE*		m	= mesh[ j ]->b_piece;
		MMDPI_PMX_MATERIAL_PTR	mpmx	= &material[ m->raw_material_id ];
		
		//	テクスチャの関連付け
		mpmx->has_texture = 0;
		if( mpmx->texture_index < texture_num )
		{
			m->raw_material->texture.copy( texture00[ mpmx->texture_index ] );
			m->has_texture = 1;
		}

		//if( mpmx->toon_texture_number < 10 )
		//	m->raw_material->toon_texture.copy( toon_texture00[ mpmx->toon_texture_number ] );
	}

	//	Pmx はモーフによる材質制御が存在する。
	//	モーフによる材質操作がある場合、モーフ稼働時のみに材質を利用するよう非透明度を変更
	//	応急処置
	for( dword i = 0; i < morph_num; i ++ )
	{
		if( morph[ i ].material == 0x00 )
			continue;
		for( dword j = 0; j < morph[ i ].offset_num; j ++ )
		{
			//float	alpha = 0;
			//for( int k = 0; k < 4; k ++ )
			//	alpha += morph[ i ].material[ j ].texture_alpha[ k ];
			uint		material_id = morph[ i ].material[ j ].material_id;
			if( material_id < material_num )
				material[ material_id ].anti_clear_rate = 1 - morph[ i ].material[ j ].diffuse[ 3 ];
		}
	}
}

//	ボーン関係処理
int mmdpiPmxAnalyze::create_bone( MMDPI_PMX_BONE_INFO_PTR pbone, uint pbone_len )
{
	mmdpiModel::bone_num = pbone_len;
	mmdpiModel::bone = new MMDPI_BONE_INFO[ mmdpiModel::bone_num ];
	
	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].parent		= 0x00;
		mmdpiModel::bone[ i ].first_child	= 0x00;
		mmdpiModel::bone[ i ].sibling		= 0x00;

		mmdpiModel::bone[ i ].level		= pbone[ i ].level;
	}

	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].id = i;
		mmdpiModel::bone[ i ].child_flag = pbone[ i ].child_flag;

		mmdpiModel::bone[ i ].init_mat.transelation( pbone[ i ].pos[ 0 ], pbone[ i ].pos[ 1 ], pbone[ i ].pos[ 2 ] );

		//	親ボーン設定
		mmdpiModel::bone[ i ].visible = 0;
		mmdpiModel::bone[ i ].length = 0;

		mmdpiModel::bone[ i ].name = new char[ strlen( pbone[ i ].name ) + 1 ];
		strcpy( mmdpiModel::bone[ i ].name, pbone[ i ].name );
		mmdpiModel::bone[ i ].sjis_name = new char[ strlen( pbone[ i ].sjis_name ) + 1 ];
		strcpy( mmdpiModel::bone[ i ].sjis_name, pbone[ i ].sjis_name );
	}

	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		ushort		bone_flag	= pbone[ i ].bone_flag;
		dword		parent_index	= pbone[ i ].parent_index;

		mmdpiModel::bone[ i ].child_bone = 0x00;
		if( mmdpiModel::bone[ i ].child_flag && pbone[ i ].child_index < mmdpiModel::bone_num )
			mmdpiModel::bone[ i ].child_bone = &mmdpiModel::bone[ pbone[ i ].child_index ]; 

		if( parent_index < mmdpiModel::bone_num )
		{
			mmdpiModel::bone[ i ].parent = &mmdpiModel::bone[ parent_index ];
			
			//	子供ボーン設定
			MMDPI_BONE_INFO_PTR*	tbone = &mmdpiModel::bone[ parent_index ].first_child;
			while( *tbone )
				tbone = &( *tbone )->sibling;
			( *tbone ) = &mmdpiModel::bone[ i ];
		}
	}

	if( mmdpiModel::bone[ 0 ].first_child == 0x00 && 1 < mmdpiModel::bone_num )
		mmdpiModel::bone[ 0 ].first_child = &mmdpiModel::bone[ 1 ];

	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].offset_mat = mmdpiModel::bone[ i ].init_mat.get_inverse();
	}
	
	//	init_mat 初期化
	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
		mmdpiModel::bone[ i ].init_mat = init_mat_calc_bottom( &mmdpiModel::bone[ i ] );

	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		if( mmdpiModel::bone[ i ].child_flag == 0 )
		{
			mmdpiModel::bone[ i ].posoffset_matrix.transelation( pbone[ i ].offset[ 0 ], -pbone[ i ].offset[ 1 ], pbone[ i ].offset[ 2 ] );
			//bone[ i ].posoffset_matrix = bone[ i ].init_mat * bone[ i ].posoffset_matrix;
		}
	}

	//	初期設定
	refresh_bone_mat();
	for( uint i = 0; i < mmdpiModel::bone_num; i ++ )
		make_global_matrix( i );
		
	return 0;
}

mmdpiPmxAnalyze::mmdpiPmxAnalyze()
{
	adjust_material = 0x00;
	adjust_vertex	= 0x00;

	texture		= 0x00;
	toon_texture	= 0x00;
	texture00	= 0x00;
	toon_texture00	= 0x00;
}

mmdpiPmxAnalyze::~mmdpiPmxAnalyze()
{
	delete[] adjust_material;
	delete adjust_vertex;

	delete[] texture;
	delete[] toon_texture;
	//if( texture00 )
	//{
	//	delete[] texture00;
	//	texture00 = 0x00;
	//}
	//if( toon_texture00 )
	//{
	//	delete[] toon_texture00;
	//	toon_texture00 = 0x00;
	//}
}

