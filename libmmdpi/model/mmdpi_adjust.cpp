
#include "mmdpi_adjust.h"


//	頂点が範囲内で使用ボーン数が上限を超えたら 1
//	頂点テクスチャの使えない状況でのメッシュ分割処理
int mmdpiAdjust::vertex_bone_over_range( uint* dev_pos, uint* using_bone_num, uint start, uint end, uint range,
						uint bone_num, dword* face, MMDPI_BLOCK_VERTEX_PTR vertex )
	//					切断位置、使用ボーン数、検索範囲*2　検索上限
{
	int*		temp_bone_list = new int[ bone_num ];	
	dword		pos_l = 0;
	int		pos_flag = 1;
	dword		using_bone_num_local;


	for( uint j = 0; j < bone_num; j ++ )
		temp_bone_list[ j ] = 0;
	temp_bone_list[ 0 ] = 1;
	using_bone_num_local = 1;
	
	//	頂点を計算
	for( uint i = start; i < end && pos_flag; i ++ )
	{
		uint	v = face[ i ];
		for( int k = 0; k < 4; k ++ )
		{
			if( temp_bone_list[ ( int )vertex->index[ v ][ k ] ] == 0 )
			{
				using_bone_num_local ++;
				temp_bone_list[ ( int )vertex->index[ v ][ k ] ] = 1;
			}
		}	

		//	頂点数が上限である
		if( using_bone_num_local >= range )
		{
			pos_l = i - i % 3;
			pos_flag = 0;
		}
	}

	delete[] temp_bone_list;

	if( using_bone_num )
		( *using_bone_num ) = using_bone_num_local;

	if( dev_pos )
		( *dev_pos ) = end;
	if( pos_flag )
		return 0;

	if( dev_pos )
		( *dev_pos ) = pos_l;
	return 1;
}

//	新マテリアルの作成
//	シェーダで使うボーン数が _MMDPI_MATERIAL_USING_BONE_NUM_ 未満の個数になるように頂点を分割
int mmdpiAdjust::adjust_material_bone( dword material_num, MMDPI_MATERIAL_PTR material,
										dword bone_num, dword* face, MMDPI_BLOCK_VERTEX_PTR vertex )
{
	//	再設定後のマテリアルカウント
	//	マテリアルごとの使用ボーンが_MMDPI_MATERIAL_USING_BONE_NUM_ - 1 以下までにする
	//	マテリアルで使用するボーンをカウント
	//	使用ボーンカウント
	dword	fver_num_base	= 0;
	b_material_num = 0;
	for( dword i = 0; i < material_num; i ++ )
	{
		uint	fver_num = material[ i ].face_num;
		uint	bone_num_temp;
		uint	fver_end = fver_num_base + fver_num;
		uint	dev_fver_num = fver_end;

		while( vertex_bone_over_range( &dev_fver_num, &bone_num_temp, 
					fver_num_base, fver_end, _MMDPI_MATERIAL_USING_BONE_NUM_,
					bone_num, face, vertex ) )
		{
			b_material_num ++;
			fver_num_base = dev_fver_num;
		}

		b_material_num ++;
		fver_num_base = dev_fver_num;
	}

	//	確保
	b_material = new MMDPI_MATERIAL[ b_material_num ];
	for( dword i = 0; i < b_material_num; i ++ )
	{
		b_material[ i ].dev_flag = new int[ b_material_num ];
		for( dword j = 0; j < b_material_num; j ++ )
			b_material[ i ].dev_flag[ j ] = 0;
	}

	//	マテリアルごとの頂点数確保
	fver_num_base = 0;
	uint	dev_flag_count = 0;		//	分割されたマテリアル数
	for( dword i = 0, j = 0; i < material_num; i ++ )
	{
		//	最初にマテリアルあたりのボーン上限を超える場所を求める
		uint	fver_num = material[ i ].face_num;
		uint	bone_num_temp;
		uint	fver_end = fver_num_base + fver_num;
		uint	dev_fver_num = fver_end;
	
		uint	dev_flag;

		dev_flag_count	= j;
		
		while( vertex_bone_over_range( &dev_fver_num, &bone_num_temp, 
				fver_num_base, fver_end, _MMDPI_MATERIAL_USING_BONE_NUM_,
				bone_num, face, vertex )
			)
		{
			//	上限を超えていたら
			b_material[ j ].face_num	= dev_fver_num - fver_num_base;		//	頂点数
			b_material[ j ].pid		= i;					//	分割前のマテリアル番号
			b_material[ j ].bone_num	= bone_num_temp;			//	必要なボーン数
			b_material[ j ].face_top	= fver_num_base;			//	faceの開始位置

			//	vertex_bone_over_range での pos_l - pos_l % 3 が原因で
			//	頂点の相互性がおかしくなるので、
			//	pos_l % 3 > 0 のときのみ、自分（頂）をコピーする
			dev_flag = j + ( ( fver_end > dev_fver_num )? 1 : 0 );
			for( uint k = dev_flag_count; k < dev_flag; k ++ )
				b_material[ j ].dev_flag[ k ] = 1;
			
			fver_num_base = dev_fver_num;
			j ++;
		}
		b_material[ j ].face_num	= dev_fver_num - fver_num_base;		//	頂点数
		b_material[ j ].pid		= i;					//	分割前のマテリアル番号
		b_material[ j ].bone_num	= bone_num_temp;			//	必要なボーン数
		b_material[ j ].face_top	= fver_num_base;			//	faceの開始位置
		
		dev_flag = j;
		for( uint k = dev_flag_count; k < dev_flag; k ++ )
			b_material[ j ].dev_flag[ k ] = 1;
		
		fver_num_base = dev_fver_num;
		j ++;
	}
	
	return 0;
}

//	他のマテリアルで重複していたら重複した頂点を増やし、重複を阻止
dword mmdpiAdjust::material_booking_vertex( vector<MMDPI_VERTEX*>* new_vertex, dword v, dword* face, dword material_indext )
{
	int			update_flag;
	dword			vertex_indexj;
	MMDPI_VERTEX_PTR	add_vertex = 0x00;

	update_flag = 0;

	//	重複マテリアルあり判定
	//	重複頂点あり判定

	//	同マテリアル上で重複があるかもしれないので、
	//	マテリアル分割されたマテリアルは頂点分割を実行
	if( b_material[ material_indext ].dev_flag[ material_indext ] )
		update_flag = 1;

	vertex_indexj = 0;
	for( dword j = 0; j < material_indext && update_flag == 0; j ++ )
	{
		if( b_material[ material_indext ].dev_flag[ j ] )
		{
			for( uint k = 0; k < b_material[ j ].face_num && update_flag == 0; k ++ )
			{
				if( face[ v ] == face[ vertex_indexj ] )
					update_flag = 1;	//	重複あり
				vertex_indexj ++;
			}
		}
		else
			vertex_indexj += b_material[ j ].face_num;
	}
	
	//	重複なし
	if( update_flag == 0 )
		return new_vertex->size();

	//	データの保存
	
	//	頂点追加
	add_vertex = new MMDPI_VERTEX();
	*add_vertex = *( *new_vertex )[ face[ v ] ];
	new_vertex->push_back( add_vertex );
	//add_vertex = ( *new_vertex )[ face[ v ] ];
	//new_vertex->push_back( add_vertex );
	
	//	インデックス更新
	face[ v ] = new_vertex->size() - 1;

	return new_vertex->size();
}

//	マテリアルごとに頂点を分割する（ブロック数は指定する）
//	（ボーン設定等で）重複する頂点をコピーして二重にする
int mmdpiAdjust::adjust_polygon( dword* face, dword face_num, MMDPI_BLOCK_VERTEX_PTR vertex, dword vertex_num )
{
	vector<MMDPI_VERTEX*>	vertex_list;
	MMDPI_VERTEX*		new_vertex;
	dword			fver_num_base = 0;

	for( dword i = 0; i < vertex_num; i ++ )
	{
		new_vertex = new MMDPI_VERTEX();

		new_vertex->ver = vertex->ver[ i ];
		new_vertex->uv  = vertex->uv [ i ];
		new_vertex->nor = vertex->nor[ i ];
		new_vertex->index = vertex->index[ i ];
		new_vertex->weight = vertex->weight[ i ];

		vertex_list.push_back( new_vertex );
	}

	//	頂点調節
	for( dword i = 0; i < b_material_num; i ++ )
	{
		uint	fver_num = b_material[ i ].face_num;
		
		//	マテリアルでの重複判定
		int		dflag = 0;
		for( uint j = 0; j < i; j ++ )
			dflag = dflag | b_material[ i ].dev_flag[ j ];

		if( dflag )
		{
			//	他のマテリアルで同じ頂点を使用していないか？
			for( dword vf = 0; vf < fver_num; vf ++ )
				material_booking_vertex( &vertex_list, vf + fver_num_base, face, i );
		}
		fver_num_base += fver_num;
	}
	b_vertex_num = vertex_list.size();
	b_vertex = new MMDPI_VERTEX[ b_vertex_num ];
	for( uint i = 0; i < b_vertex_num; i ++ )
	{
		b_vertex[ i ] = *vertex_list[ i ];
				
		delete vertex_list[ i ];
		vertex_list[ i ] = 0x00;
	}

	return 0;
}

//	頂点インデックスが ushort で扱えるように調節
//	ushort の上限を超えた場合には分割する。
int mmdpiAdjust::adjust_face( dword* face, dword face_num, dword vertex_num )
{
	dword			vertex_range = 0xffff - 0xffff % 3;		//	65532 は 65532 mod 3 == 0 で 65536 に近い数
	vector< dword >		new_face_pos;				//	頂点集合区切り位置
	
	//	分割位置と個数をカウント
	new_face_pos.push_back( 0 );
	for( dword k = 0; k < b_material_num; k ++ )
	{		
		dword	face_pos = b_material[ k ].face_top;

		for( dword j = 0; j < b_material[ k ].face_num; j += 3 )
		{
			int		bid[ 3 ];
			int		jp = j + face_pos;

			//	所属する頂点集合を計算
			for( int i = 0; i < 3; i ++ )
				bid[ i ] = face[ jp + i ] / vertex_range;

			if( bid[ 0 ] == bid[ 1 ] && bid[ 0 ] == bid[ 2 ] )
				;
			else	//	頂点集合が違う３対が来たら頂点集合区切り位置を増やす
			{
				new_face_pos.push_back( face_pos );
				break;
			}
		}
		
		b_material[ k ].face_id = new_face_pos.size() - 1;
	}
	new_face_pos.push_back( face_num );

	//	インデックス設定
	b_face_num = new_face_pos.size() - 1;
	b_face = new MMDPI_BLOCK_FACE[ b_face_num ];
	for( dword j = 0; j < b_face_num; j ++ )
	{
		//	使用するシェーダバッファ保存
		b_face[ j ].face_divide_index = new_face_pos[ j ];
		b_face[ j ].buffer_id = j;
		
		dword	s_face_pos = new_face_pos[ j + 0 ];
		dword	e_face_pos = new_face_pos[ j + 1 ];
	
		dword	max_vid = 0, min_vid = vertex_num;
		for( dword i = s_face_pos; i < e_face_pos; i ++ )
		{
			dword	f = face[ i ];
			if( max_vid < f )
				max_vid = f;
			if( min_vid > f )
				min_vid = f;
		}

		b_face[ j ].face_num = e_face_pos - s_face_pos;
		b_face[ j ].face = new ushort[ b_face[ j ].face_num ];
		for( dword i = s_face_pos, c = 0; c < b_face[ j ].face_num; i ++, c ++ )
			b_face[ j ].face[ c ] = ( ushort )( face[ i ] - min_vid );

		b_face[ j ].vertex_num = max_vid - min_vid + 1;
		b_face[ j ].vertex = new MMDPI_VERTEX[ b_face[ j ].vertex_num ];

		//	頂点の再配置
		for( dword i = min_vid, c = 0; c < b_face[ j ].vertex_num; i ++, c ++ )
			b_face[ j ].vertex[ c ] = b_vertex[ i ];
	}

	//	face 下にmaterial が所属する
	for( dword j = 0; j < b_face_num; j ++ )
		b_face[ j ].material_num = 0;
	for( dword i = 0; i < b_material_num; i ++ )
		b_face[ b_material[ i ].face_id ].material_num ++;
	for( dword j = 0; j < b_face_num; j ++ )
	{
		dword	c = 0;
		for( dword i = 0; i < b_material_num; i ++ )
		{
			if( j == b_material[ i ].face_id )
				c ++;
		}
		b_face[ j ].material_num = c;
		b_face[ j ].material = new MMDPI_MATERIAL_PTR[ b_face[ j ].material_num ];
		c = 0;
		for( dword i = 0; i < b_material_num; i ++ )
		{
			if( j == b_material[ i ].face_id )
			{
				b_face[ j ].material[ c ] = &b_material[ i ];
				c ++;
			}
		}
	}

	dword	fver_num_base = b_material[ 0 ].face_num;
	for( dword i = 1; i < b_material_num; i ++ )
	{	
		if( b_material[ i ].face_id != b_material[ i - 1 ].face_id )
			fver_num_base = 0;
		
		b_material[ i ].face_top = fver_num_base;
		fver_num_base += b_material[ i ].face_num;
	}

	return 0;
}

//	ボーンの最適化
int mmdpiAdjust::adjust_bone( void )
{
	for( uint i = 0; i < b_material_num; i ++ )
	{
		for( int j = 0; j < _MMDPI_MATERIAL_USING_BONE_NUM_; j ++ )
			b_material[ i ].bone_list[ j ] = 0;
		b_material[ i ].bone_list_num = 1;

		for( uint vi = 0; vi < b_material[ i ].face_num; vi ++ )
		{
			MMDPI_BLOCK_FACE_PTR	face	= &b_face[ b_material[ i ].face_id ];
			dword			vif	= face->face[ vi + b_material[ i ].face_top ];
			
			if( face->vertex[ vif ].update_flag )
				continue;

			for( int k = 0; k < 4; k ++ )
			{
				int		index = -1;
				long	vertex_index;
				
				vertex_index = ( long )face->vertex[ vif ].index[ k ];

				//	リストに登録済みか確認
				for( dword j = 0; j < b_material[ i ].bone_list_num && index < 0; j ++ )
				{
					if( ( long )b_material[ i ].bone_list[ j ] == vertex_index )
						index = j;
				}

				//	リストにない場合、リストに登録
				if( index < 0 && b_material[ i ].bone_list_num < _MMDPI_MATERIAL_USING_BONE_NUM_ )
				{
					index = b_material[ i ].bone_list_num;
					b_material[ i ].bone_list[ index ] = vertex_index;
					b_material[ i ].bone_list_num ++;
				}
			
				face->vertex[ vif ].index[ k ] = ( float )index;
			}

			//	頂点の調節がすべて終了
			//	二重変更の阻止
			face->vertex[ vif ].update_flag = 1;
		}
	}

	return 0;
}

void mmdpiAdjust::update_matrix( MMDPI_BONE_INFO_PTR bone, dword bone_num )
{
	for( dword i = 0; i < b_material_num; i ++ )
	{
		for( uint j = 0; j < b_material[ i ].bone_list_num; j ++ )
		{
			int	bone_index = ( int )b_material[ i ].bone_list[ j ];

			b_material[ i ].matrix[ j ] = bone[ bone_index ].local_matrix;
		}
	}
}

mmdpiAdjust::mmdpiAdjust()
{
	b_vertex	= 0x00;
	b_face		= 0x00;
	b_material	= 0x00;
}

mmdpiAdjust::~mmdpiAdjust()
{
	delete[] b_vertex;
	if( b_face )
	{
		for( dword i = 0; i < b_face_num; i ++ )
		{
			delete[] b_face[ i ].face;
			delete[] b_face[ i ].material;
			delete[] b_face[ i ].vertex;
		}
		delete[] b_face;
	}
	delete[] b_material;
}