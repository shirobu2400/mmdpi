
#include "pmd_analyze.hpp"


// 読み込み
int mmdpiPmdAnalyze::load( const char *file_name )
{
	if( this->mmdpiPmdLoad::load( file_name ) )
		return -1;
	if( this->mmdpiModel::create() )
		return -1;
	return this->mmdpiPmdAnalyze::analyze();
}

// PMD解析
int mmdpiPmdAnalyze::analyze( void )
{
	adjust_material = new MMDPI_MATERIAL[ this->material_num ];
	if( adjust_material == 0x00 )
		return -1;

	dword	face_top = 0;
	for( dword i = 0; i < this->material_num; i ++ )
	{
		this->adjust_material[ i ].face_top = face_top;
		this->adjust_material[ i ].face_num = this->material[ i ].face_vert_count;
		face_top += this->material[ i ].face_vert_count;
	}

	this->adjust_vertex = new MMDPI_BLOCK_VERTEX();
	if( this->adjust_vertex == 0x00 )
		return -1;
	this->adjust_vertex->alloc( this->mmdpiPmdLoad::vertex_num );

	for( dword i = 0; i < this->mmdpiPmdLoad::vertex_num; i ++ )
	{
		MMDPI_PMD_VERTEX_PTR	ver = &this->mmdpiPmdLoad::vertex[ i ];

		this->adjust_vertex->ver[ i ].x = ver->pos[ 0 ];
		this->adjust_vertex->ver[ i ].y = ver->pos[ 1 ];
		this->adjust_vertex->ver[ i ].z = ver->pos[ 2 ];

		this->adjust_vertex->uv[ i ].x = ver->uv[ 0 ];
		this->adjust_vertex->uv[ i ].y = 1 - ver->uv[ 1 ];
		this->adjust_vertex->uv[ i ].z = 0;
		this->adjust_vertex->uv[ i ].w = 0;

		this->adjust_vertex->nor[ i ].x = ver->nor[ 0 ];
		this->adjust_vertex->nor[ i ].y = ver->nor[ 1 ];
		this->adjust_vertex->nor[ i ].z = ver->nor[ 2 ];

		// ボーン
		this->adjust_vertex->index[ i ].x = ( float )ver->bone_num[ 0 ];
		this->adjust_vertex->index[ i ].y = ( float )ver->bone_num[ 1 ];
		this->adjust_vertex->index[ i ].z = ( float )0;
		this->adjust_vertex->index[ i ].w = ( float )0;

		// 重み
		this->adjust_vertex->weight[ i ].x = ver->bone_weight / 100.0f;
		this->adjust_vertex->weight[ i ].y = 1 - this->adjust_vertex->weight[ i ].x;
		this->adjust_vertex->weight[ i ].z = 0;
		this->adjust_vertex->weight[ i ].w = 0;
	}

	// 最適化
	mmdpiAdjust::adjust( this->adjust_vertex, this->mmdpiPmdLoad::vertex_num,
				this->face,this-> face_num,
				this->adjust_material, this->material_num,
				mmdpiBone::bone, mmdpiPmdLoad::bone_num );

	// テクスチャ
	this->load_texture();

	//skin = new MMDPI_SKIN_INFO[ skin_num ];
	//for( uint i = 0; i < skin_num; i ++ )
	//{
	//	a_skin[ i ].skin_flag = 0;
	//	a_skin[ i ].skin_name = skin[ i ].skin_name;
	//}

	// マテリアル情報の保存
	for( dword j = 0; j < this->mesh.size(); j ++ )
	{
		MMDPI_PIECE*			m	= this->mesh[ j ]->b_piece;
		MMDPI_PMD_MATERIAL_PTR		mpmx	= &mmdpiPmdLoad::material[ m->raw_material_id ];

		m->edge_size = mpmx->edge_flag * 0.02f;

		m->edge_color.r = 0;
		m->edge_color.g = 0;
		m->edge_color.b = 0;
		m->edge_color.a = 1;

		m->opacity = mpmx->alpha;

		m->color.r = mpmx->diffuse_color[ 0 ];
		m->color.g = mpmx->diffuse_color[ 1 ];
		m->color.b = mpmx->diffuse_color[ 2 ];
		m->color.a = m->opacity;
		if( m->has_texture )
			m->color.a = 0;

		m->is_draw_both = 0;
		if( m->opacity < 1 - 1e-8 )
			m->is_draw_both = 1;
	}

	return 0;
}

void mmdpiPmdAnalyze::load_texture( void )
{
	// テクスチャ
	dword	fver_num_base = 0;
	long	ppid = -1;
	dword	pi = 0;

	// 一時的に読み込み
	this->texture = new MMDPI_IMAGE[ this->material_num ];
	if( this->texture == 0x00 )
		return ;

	this->toon_texture = new MMDPI_IMAGE[ this->material_num ];
	if( this->toon_texture == 0x00 )
		return ;

	// 読み込み済みテクスチャのリスト
	char**	loaded_texture_name = new char*[ this->material_num ];
	if( loaded_texture_name == 0x00 )
		return ;
	for( uint i = 0; i < this->material_num; i ++ )
	{
		loaded_texture_name[ i ] = new char[ 0x200 ];
		for( uint j = 0; j < 0x200; j ++ )
			loaded_texture_name[ i ][ j ] = '\0';
	}

	for( dword i = 0; i < mesh.size(); i ++ )
	{
		MMDPI_PIECE*		m	= this->mesh[ i ]->b_piece;
		MMDPI_PMD_MATERIAL_PTR	mpmx	= &mmdpiPmdLoad::material[ m->raw_material_id ];
		int			ri;
		char*			texture_file_name;

		texture_file_name = mpmx->texture_file_name;

		char	texture_file_name_full[ 0x1000 ];
		int	j, k;
		for( k = 0; mmdpiPmdLoad::directory[ k ]; k ++ )
			texture_file_name_full[ k ] = mmdpiPmdLoad::directory[ k ];

		for( j = 0;
			texture_file_name[ j ]
			&& texture_file_name[ j ] != '*';
			j ++, k ++
		)
			texture_file_name_full[ k ] = texture_file_name[ j ];
		texture_file_name_full[ k ] = '\0';

		// 今までにテクスチャが読み込まれていればその位置を取得
		for( ri = m->raw_material_id; ri >= 0; ri -- )
		{
			if( strcmp( loaded_texture_name[ ri ], texture_file_name_full ) == 0 )
				break;
		}
		if( ri < 0 )	// 読み込まれたことがない
		{
			ri = m->raw_material_id;
			strcpy( loaded_texture_name[ ri ], texture_file_name_full );
			texture[ ri ].load( texture_file_name_full );
		}

		// テクスチャの関連付け
		m->raw_material->texture.copy( texture[ ri ] );
		if( texture[ ri ].type )
			m->has_texture = 1;
	}
	for( uint i = 0; i < material_num; i ++ )
		delete[] loaded_texture_name[ i ];
	delete[] loaded_texture_name;
}

// ボーン関係処理
int mmdpiPmdAnalyze::create_bone( MMDPI_PMD_BONE_INFO_PTR pbone, uint pbone_len )
{
	mmdpiModel::bone_num = pbone_len;
	mmdpiModel::bone = new MMDPI_BONE_INFO[ mmdpiModel::bone_num ];

	for( dword i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].parent		= 0x00;
		mmdpiModel::bone[ i ].first_child	= 0x00;
		mmdpiModel::bone[ i ].sibling		= 0x00;
	}

	for( dword i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].id = i;

		// 親ボーン設定
		if( pbone[ i ].parent_bone_index < mmdpiModel::bone_num )
			mmdpiModel::bone[ i ].parent = &mmdpiModel::bone[ pbone[ i ].parent_bone_index ];

		// 子供ボーン設定
		int child_id = -1;
		for( dword j = 0; j < mmdpiModel::bone_num && child_id == -1; j ++ )
		{
			if( pbone[ j ].parent_bone_index == i )
				child_id = j;
		}
		if( child_id != -1 )
			mmdpiModel::bone[ i ].first_child = &mmdpiModel::bone[ child_id ];

		// 兄弟ボーン設定
		int sibling_id = -1;
		for( dword j = i + 1; j < mmdpiModel::bone_num && sibling_id == -1; j ++ )
		{
			if( pbone[ j ].parent_bone_index == pbone[ i ].parent_bone_index )
				sibling_id = j;
		}
		if( sibling_id != -1 )
			mmdpiModel::bone[ i ].sibling = &mmdpiModel::bone[ sibling_id ];

		mmdpiModel::bone[ i ].init_matrix.transelation( pbone[ i ].bone_head_pos[ 0 ], pbone[ i ].bone_head_pos[ 1 ], pbone[ i ].bone_head_pos[ 2 ] );
		mmdpiModel::bone[ i ].visible = 0;
		mmdpiModel::bone[ i ].length = 0;

		mmdpiModel::bone[ i ].name = new char[ 32 ];
		strcpy( mmdpiModel::bone[ i ].name, pbone[ i ].bone_name );
		mmdpiModel::bone[ i ].name[ 20 ] = '\0';
	}

	for( dword i = 0; i < mmdpiModel::bone_num; i ++ )
		mmdpiModel::bone[ i ].offset_matrix = mmdpiModel::bone[ i ].init_matrix.get_inverse();

	// init_mat 初期化
	mmdpiBone::compute_init_matrix( &mmdpiModel::bone[ 0 ], 0x00 );

	// 初期設定
	mmdpiBone::refresh_bone_mat();
	mmdpiBone::update_global_matrix( &mmdpiModel::bone[ 0 ], 0x00 );

	return 0;
}

mmdpiPmdAnalyze::mmdpiPmdAnalyze()
{
	this->adjust_material = 0x00;
	this->adjust_vertex	= 0x00;

	this->texture		= 0x00;
	this->toon_texture	= 0x00;
}

mmdpiPmdAnalyze::~mmdpiPmdAnalyze()
{
	delete[] this->adjust_material;
	delete this->adjust_vertex;

	delete[] this->texture;
	delete[] this->toon_texture;
}
