
#include "mmdpi_adjust.h"



int mmdpiAdjust::adjust( MMDPI_BLOCK_VERTEX* vertex, dword vertex_num, 
			dword* face, dword face_num,
			MMDPI_MATERIAL_PTR material, dword material_num,
			MMDPI_BONE_INFO_PTR bone, dword bone_num )
{
	//	１メッシュの扱える頂点インデックスの上限
	dword		vertex_range = 0xffff - 0xffff % 3;

	//	face and vertex adjust
	//	条件は 
	//	_MMDPI_MATERIAL_USING_BONE_NUM_ を超えないボーンを保持
	//	vartex の個数を vertex_range 以下にする
	//	face は新しく作り直す
	int	bone_counter	= 1;	// id:0 のボーンはどのメッシュにも存在させる
	dword	vartexid_min	= 0;
	dword	vartexid_max	= 0;

	//	ボーンを登録済みかのフラグ
	dword*	bone_list = new dword[ bone_num ];
	if( bone_list == 0x00 )
		return -1;
	for( dword i = 0; i < bone_num; i ++ )
		bone_list[ i ] = 0;
	bone_list[ 0 ] = 1;	// id:0 のボーンはどのメッシュにも存在させる

	//	生データの頂点から新しい頂点を求めるリスト
	//	未登録時は -1
	int*	oldv_2_newv = new int[ face_num ];
	if( oldv_2_newv == 0x00 )
		return 0;
	for( dword i = 0; i < face_num; i ++ )
		oldv_2_newv[ i ] = -1;

	//	生の頂点ボーンのデータから新しく作り直した頂点ボーンの対応表
	int*	rawbone_2_newbone = new int[ bone_num ];
	if( rawbone_2_newbone == 0x00 )
		return 0;
	for( dword i = 0; i < bone_num; i ++ )
		rawbone_2_newbone[ i ] = 0;

	vector<MMDPI_VERTEX>		new_vertex;
	vector<mmdpiShaderIndex>	new_face;

	//	描画時に必要なボーンのリスト
	dword				new_bone_list[ _MMDPI_MATERIAL_USING_BONE_NUM_ ];
	for( uint i = 0; i < _MMDPI_MATERIAL_USING_BONE_NUM_; i ++ )
		new_bone_list[ i ] = 0;

	int				material_id = 0;
	uint				v3i_max = 0;

	//	メッシュ更新フラグ
	int				update_flag = 0x00;

	for( dword f = 0; f < face_num; f += 3 )
	{
		//	頂点インデックス
		dword			v3i[ 3 ] = { face[ f + 0 ], face[ f + 1 ], face[ f + 2 ] };

		//	頂点情報作業領域
		MMDPI_VERTEX		v;

	
		//	頂点番号の最大値と最小値を計算
		//	使用する頂点の番号の範囲を検出
		for( int i = 0; i < 3; i ++ )
		{
			if( vartexid_min > v3i[ i ] )
				vartexid_min = v3i[ i ];
			if( vartexid_max < v3i[ i ] )
				vartexid_max = v3i[ i ];
		}

		//	頂点数が範囲外
		if( vartexid_max - vartexid_min > vertex_range )
			update_flag |= 0x01;
		
		//	このメッシュで使用するボーンが上限に達した
		if( bone_counter >= _MMDPI_MATERIAL_USING_BONE_NUM_ - 4 )
			update_flag |= 0x02;
		
		//	マテリアルが更新された
		if( f >= material[ material_id ].face_top + material[ material_id ].face_num - 3 )
			update_flag |= 0x04;

		//	メッシュを新しく作成
		if( update_flag )
		{
			mmdpiMesh*	mesh_temp = new mmdpiMesh( this );
			if( mesh_temp == 0x00 )
				return -1;

			//	メッシュ設定
			mesh_temp->id = mesh.size();
			mesh_temp->set_vertex( &new_vertex[ 0 ], new_vertex.size() );	// &vector array[ 0 ] でベクターを配列化
			mesh_temp->set_face( &new_face[ 0 ], new_face.size() );
			mesh_temp->set_material( material_id, &material[ material_id ] );
			mesh_temp->set_boneid( new_bone_list, bone_counter );

			//	メッシュを追加
			mesh.push_back( mesh_temp );

			//	マテリアルが更新された
			if( update_flag & 0x04 )
				material_id ++;

			//	各種初期化
			new_face.clear();
			new_vertex.clear();

			//	登録済みボーン表を初期化
			for( dword i = 0; i < bone_num; i ++ )
				bone_list[ i ] = 0;
			bone_list[ 0 ] = 1;	// id:0 のボーンはどのメッシュにも存在させる

			//	ボーン対応表を初期化
			bone_counter ++;
			for( int i = 0; i < bone_counter; i ++ )
				new_bone_list[ i ] = 0;
			bone_counter = 1;	// id:0 のボーンはどのメッシュにも存在させる
			
			for( dword i = 0; i < bone_num; i ++ )
				rawbone_2_newbone[ i ] = 0;

			//	頂点リストを初期化
			//	全ての頂点を未設定へ
			v3i_max ++;
			for( dword i = 0; i < face_num; i ++ )
				oldv_2_newv[ i ] = -1;
			v3i_max = 0;

			//	頂点番号の現在の値を設定
			if( f + 4 < face_num )
			{
				vartexid_min = face[ f + 4 ];
				vartexid_max = face[ f + 4 ];
			}

			//	メッシュを更新情報の初期化
			update_flag = 0x00;
		}

		//	メッシュ操作
		for( int i = 0; i < 3; i ++ )
		{
			uint	new_vi		= new_vertex.size();
			uint	old_vi		= v3i[ i ];

			if( oldv_2_newv[ old_vi ] < 0 )
			{
				//	新たに頂点を追加
				v.ver		= vertex->ver[ old_vi ];
				v.uv		= vertex->uv[ old_vi ];
				v.nor		= vertex->nor[ old_vi ];
				v.weight	= vertex->weight[ old_vi ];

				v.index[ 0 ]	= 0;
				v.index[ 1 ]	= 0;
				v.index[ 2 ]	= 0;
				v.index[ 3 ]	= 0;


				//	生データの頂点から新しい頂点を求めるリストに新しい頂点の位置を保存
				oldv_2_newv[ old_vi ] = new_vi;

				//	ボーンリストの更新
				for( int j = 0; j < 4; j ++ )
				{
					int	bone_index = ( int )( vertex->index[ old_vi ][ j ] + 0.1 );
					if( bone_index < 0 )
						bone_index = 0;

					//	ボーンが登録済み
					if( bone_list[ bone_index ] == 0 )
					{
						//	new bone index to old bone index
						new_bone_list[ bone_counter ] = bone_index;

						//	old bone index to new bone index
						rawbone_2_newbone[ bone_index ] = bone_counter;

						bone_counter ++;
					}

					//	ボーンを指定
					v.index[ j ] = ( float )( rawbone_2_newbone[ bone_index ] + 1e-4f );

					//	ボーンを登録済みにする
					bone_list[ bone_index ] ++;
				}
					
				//	頂点追加
				new_vertex.push_back( v );
			}
			else
			{
				//	頂点は追加済み
				new_vi = oldv_2_newv[ old_vi ];
			}

			//	使用した領域の最大値を保存
			//	あとで初期化するときに高速化のため必要
			if( v3i_max < old_vi )
				v3i_max = old_vi;

			if( new_vi >= vertex_range )
				update_flag |= 0x01;

			//	頂点と頂点インデックスの関連づけ
			new_face.push_back( new_vi );
		}
	}

	delete[] oldv_2_newv;
	delete[] bone_list;
	
	return 0;
}

mmdpiAdjust::mmdpiAdjust()
{
}

mmdpiAdjust::~mmdpiAdjust()
{
}