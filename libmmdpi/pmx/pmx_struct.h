
#include "../model/mmdpi_struct.h"

#pragma once


//#pragma pack( push, 1 )	//アラインメント制御をオフる

// ヘッダ
typedef struct tagMMDPI_PMX_HEADER
{
	char	pmx[ 4 ];			//	Header
	float	version;			//	バージョン
	BYTE	byte_num;			//	後続するデータ列のバイトサイズ  PMX2.0は 8 で固定
	BYTE	*byte;			
	//	n : byte[8]	| byte * バイトサイズ

	//バイト列 - byte
	//  [0] - エンコード方式  | 0:UTF16 1:UTF8
	//  [1] - 追加UV数 	| 0～4 詳細は頂点参照
	//  [2] - 頂点Indexサイズ | 1,2,4 のいずれか
	//  [3] - テクスチャIndexサイズ | 1,2,4 のいずれか
	//  [4] - 材質Indexサイズ | 1,2,4 のいずれか
	//  [5] - ボーンIndexサイズ | 1,2,4 のいずれか
	//  [6] - モーフIndexサイズ | 1,2,4 のいずれか
	//  [7] - 剛体Indexサイズ | 1,2,4 のいずれか

	char*	name;			//	名前
	char*	name_eng;		//	名前英語
	char*	comment;		//	コメント　		著作権情報
	char*	comment_eng;	//	コメント英語　	著作権情報

} MMDPI_PMX_HEADER;

// 頂点
typedef struct tagMMDPI_PMX_VERTEX	
{
	float	pos[ 3 ];		//	x, y, z
	float	nor[ 3 ];		//	法線
	float	uv[ 2 ];		//	uv
	float*	add_uv;			//	追加UV
	BYTE	weight_calc;	//	ウェイト変形方式 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF
	//BDEF1 ->
	//  n : ボーンIndexサイズ  | ウェイト1.0の単一ボーン(参照Index)

	//BDEF2 ->
	//  n : ボーンIndexサイズ  | ボーン1の参照Index
	//  n : ボーンIndexサイズ  | ボーン2の参照Index
	//  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト

	//BDEF4 ->
	//  n : ボーンIndexサイズ  | ボーン1の参照Index
	//  n : ボーンIndexサイズ  | ボーン2の参照Index
	//  n : ボーンIndexサイズ  | ボーン3の参照Index
	//  n : ボーンIndexサイズ  | ボーン4の参照Index
	//  4 : float              | ボーン1のウェイト値
	//  4 : float              | ボーン2のウェイト値
	//  4 : float              | ボーン3のウェイト値
	//  4 : float              | ボーン4のウェイト値 (ウェイト計1.0の保障はない)

	//SDEF ->
	//  n : ボーンIndexサイズ  | ボーン1の参照Index
	//  n : ボーンIndexサイズ  | ボーン2の参照Index
	//  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト
	// 12 : float3             | SDEF-C値(x,y,z)
	// 12 : float3             | SDEF-R0値(x,y,z)
	// 12 : float3             | SDEF-R1値(x,y,z) ※修正値を要計算
	float	sdef_options[ 3 * 3 ];

	dword	bone_index[ 4 ];
	float	bone_value[ 4 ];

	float	edge_scale;		// エッジ倍率

	tagMMDPI_PMX_VERTEX()
	{
		memset( sdef_options, 0, sizeof( float ) * ( 3 * 3 + 1 ) );
		pos[ 0 ] = pos[ 1 ] = pos[ 2 ] = 0;
	}

} MMDPI_PMX_VERTEX, *MMDPI_PMX_VERTEX_PTR;

//	面
typedef dword MMDPI_PMX_FACE, *MMDPI_PMX_FACE_PTR;

//	テクスチャ
typedef struct tagMMDPI_PMX_TEXTURE
{

	char*	name;
	char*	sjis_name;

} MMDPI_PMX_TEXTURE, *MMDPI_PMX_TEXTURE_PTR;

//	材質
typedef struct tagMMDPI_PMX_MATERIAL
{

	char*	name;				//	材質名
	char*	eng_name;			//	材質名(英)

	float	Diffuse[ 4 ];		//	
	float	Specular[ 3 ];		//
	float	Specular_scale;		//	Specular係数
	float	Ambient[ 3 ];		//

	BYTE	bitFlag;			
	//描画フラグ(8bit) - 各bit 0:OFF 1:ON
	//0x01:両面描画, 0x02:地面影, 0x04:セルフシャドウマップへの描画, 0x08:セルフシャドウの描画, 
	//0x10:エッジ描画

	float	edge_color[ 4 ];	//	エッジ色
	float	edge_size;			//	エッジサイズ

	dword	texture_index;			//	通常テクスチャ, テクスチャテーブルの参照Index
	dword	sphere_texture_index;		//	スフィアテクスチャ, テクスチャテーブルの参照Index  ※テクスチャ拡張子の制限なし

	BYTE	sphere_mode;				
	//	スフィアモード 0:無効 1:乗算(sph) 2:加算(spa) 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)

	BYTE	toon_flag;			//	共有Toonフラグ 0:継続値は個別Toon 1:継続値は共有Toon

	dword	toon_texture_number;
	//共有Toonフラグ:0 の場合
	// n : テクスチャIndexサイズ     | Toonテクスチャ, テクスチャテーブルの参照Index
	//
	//共有Toonフラグ:1 の場合
	// 1 : byte	| 共有Toonテクスチャ[0～9] -> それぞれ toon01.bmp～toon10.bmp に対応
	//--
	//↑どちらか一方なので注意

	char*	comment;			//	メモ : 自由欄／スクリプト記述／エフェクトへのパラメータ配置など

	dword	fver_num;			//	材質に対応する面(頂点)数 (必ず3の倍数になる)

	char*	toon_name;
	char*	toon_name_sjis;

	float	anti_clear_rate;	//	非透明度 透明:0 - 1:非透明

} MMDPI_PMX_MATERIAL, *MMDPI_PMX_MATERIAL_PTR;

//	ik
typedef struct tagMMDPI_PMX_IK_INFO
{

	dword	ik_bone_index;
	BYTE	rotate_limit_flag;

	float	bottom[ 3 ];
	float	top[ 3 ];

	tagMMDPI_PMX_IK_INFO()
	{
		ik_bone_index = 0;
		rotate_limit_flag = 0;
		bottom[ 0 ] = bottom[ 1 ] = bottom[ 2 ] = 0;
		top[ 0 ] = top[ 1 ] = top[ 2 ] = 0;
	}

} MMDPI_PMX_IK_INFO, *MMDPI_PMX_IK_INFO_PTR;

//	Bone
typedef struct tagMMDPI_PMX_BONE_INFO
{

	char*	name;				//	ボーン名
	char*	eng_name;			//	ボーン名(英)

	float	pos[ 3 ];			//	位置
	dword	parent_index;			//	親ボーンのボーンIndex
	dword	level;				//	変形階層	4byte

	ushort	bone_flag;		//	ボーンフラグ(16bit) 各bit 0:OFF 1:ON
	//○ボーンフラグ
	// 0x0001  : 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
	//
	// 0x0002  : 回転可能
	// 0x0004  : 移動可能
	// 0x0008  : 表示
	// 0x0010  : 操作可
	//
	// 0x0020  : IK
	//
	// 0x0100  : 回転付与
	// 0x0200  : 移動付与
	//
	// 0x0400  : 軸固定
	// 0x0800  : ローカル軸
	//
	// 0x1000  : 物理後変形
	// 0x2000  : 外部親変形

	//	接続先:0 の場合
	float	offset[ 3 ];

	//	接続先:1 の場合
	dword	child_index;		//	接続先ボーンのボーンIndex

	//	回転付与:1 または 移動付与:1 の場合
	dword	tr_parent_index;			//	付与親ボーンのボーンIndex
	float	tr_parent_rate;			//	付与率

	//	軸固定:1 の場合
	float	axis_vector[ 3 ];		//	軸の方向ベクトル

	//	ローカル軸:1 の場合
	float	local_axis_x[ 3 ];		//	X軸の方向ベクトル
	float	local_axis_z[ 3 ];		//	Z軸の方向ベクトル ※フレーム軸算出方法は後述

	//	外部親変形:1 の場合
	dword	key_value;				//	Key値	4byte

	//IK:1 の場合 IKデータを格納
	dword	ik_target_bone_index;
	dword	ik_loop_num;

	float	ik_radius_range;		//	単位回転角度
	dword	ik_link_num;
	// n  : ボーンIndexサイズ  | IKターゲットボーンのボーンIndex
	// 4  : int  	| IKループ回数 (PMD及びMMD環境では255回が最大になるようです)
	// 4  : float	| IKループ計算時の1回あたりの制限角度 -> ラジアン角 | PMDのIK値とは4倍異なるので注意
	//
	// 4  : int  	| IKリンク数 : 後続の要素数

	MMDPI_PMX_IK_INFO_PTR	ik_link;
	// <IKリンク>
	//   n  : ボーンIndexサイズ  | リンクボーンのボーンIndex
	//   1  : byte	| 角度制限 0:OFF 1:ON
	//   ----
	//   角度制限:1の場合
	//     12 : float3	| 下限 (x,y,z) -> ラジアン角
	//     12 : float3	| 上限 (x,y,z) -> ラジアン角
	//   ----
	// </IKリンク>
	// * IKリンク数

	//	設定情報
	//	生データにはなし
	BYTE	child_flag;			//	接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
	BYTE	rotation_flag;		//	回転可能
	BYTE	translate_flag;		//	移動可能
	BYTE	show_flag;			//	表示
	BYTE	user_update_flag;	//	操作可
	BYTE	ik_flag;			//	IK
	BYTE	rotation_s_flag;	//	回転付与
	BYTE	translate_s_flag;	//	移動付与
	BYTE	const_axis_flag;	//	軸固定
	BYTE	local_axis_flag;	//	ローカル軸
	BYTE	physical_update_flag;//	物理後変形
	BYTE	out_parent_update_flag;//	外部親変形

	tagMMDPI_PMX_BONE_INFO()
	{
		ik_loop_num = 0;
		offset[ 0 ] = offset[ 1 ] = offset[ 2 ] = 0;
		child_index = ~0;

		child_flag = 0;		//	接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
		rotation_flag = 0;		//	回転可能
		translate_flag = 0;		//	移動可能
		show_flag = 0;			//	表示
		user_update_flag = 0;	//	操作可
		ik_flag = 0;			//	IK
		rotation_s_flag = 0;	//	回転付与
		translate_s_flag = 0;	//	移動付与
		const_axis_flag = 0;	//	軸固定
		local_axis_flag = 0;	//	ローカル軸
		physical_update_flag = 0;//	物理後変形
		out_parent_update_flag = 0;//	外部親変形
	}

	char*	sjis_name;

} MMDPI_PMX_BONE_INFO, *MMDPI_PMX_BONE_INFO_PTR;


//	モーフ
typedef struct tagMMDPI_PMX_MORPH_INFO_VERTEX
{

	//	頂点
	dword		vertex_id;
	float		vertex[ 3 ];

} MMDPI_PMX_MORPH_INFO_VERTEX, *MMDPI_PMX_MORPH_INFO_VERTEX_PTR;

typedef struct tagMMDPI_PMX_MORPH_INFO_UV
{

	//	ＵＶ
	BYTE		uv_number;	//	uv1, uv2 ..

	dword		uv_id;
	float		uv[ 4 ];
	
} MMDPI_PMX_MORPH_INFO_UV, *MMDPI_PMX_MORPH_INFO_UV_PTR;

typedef struct tagMMDPI_PMX_MORPH_INFO_BONE
{

	//	ボーン
	dword		bone_id;
	float		translate[ 3 ];
	float		rotation[ 4 ];

} MMDPI_PMX_MORPH_INFO_BONE, *MMDPI_PMX_MORPH_INFO_BONE_PTR;

typedef struct tagMMDPI_PMX_MORPH_INFO_MATERIAL
{

	//	材質
	dword		material_id;
	BYTE		calc_format;	//	演算法
	float		diffuse[ 4 ];
	float		specular[ 3 ];
	float		specular_alpha;
	float		ambient[ 3 ];
	float		edge_color[ 2 ];
	float		edge_size;
	float		texture_alpha[ 4 ];
	float		sphere_alpha[ 4 ];
	float		toon_texture_alpha[ 4 ];

} MMDPI_PMX_MORPH_INFO_MATERIAL, *MMDPI_PMX_MORPH_INFO_MATERIAL_PTR;

typedef struct tagMMDPI_PMX_MORPH_INFO_GROUP
{

	dword		group_id;
	float		morph_rate;

} MMDPI_PMX_MORPH_INFO_GROUP, *MMDPI_PMX_MORPH_INFO_GROUP_PTR;


//	モーフ
typedef struct tagMMDPI_PMX_MORPH_INFO
{
	char*		name;
	char*		eng_name;

	BYTE		panel;
	BYTE		type;

	dword		offset_num;

	MMDPI_PMX_MORPH_INFO_VERTEX_PTR		vertex;
	MMDPI_PMX_MORPH_INFO_UV_PTR			uv;
	MMDPI_PMX_MORPH_INFO_BONE_PTR		bone;
	MMDPI_PMX_MORPH_INFO_MATERIAL_PTR	material;
	MMDPI_PMX_MORPH_INFO_GROUP_PTR		group;

	tagMMDPI_PMX_MORPH_INFO()
	{
		vertex		= NULL;
		uv			= NULL;
		bone		= NULL;
		material	= NULL;
		group		= NULL;
	}

	//○頂点モーフ
	//n  : 頂点Indexサイズ  | 頂点Index
	//12 : float3	| 座標オフセット量(x,y,z)
	//
	//○UVモーフ
	//n  : 頂点Indexサイズ  | 頂点Index
	//16 : float4	| UVオフセット量(x,y,z,w) ※通常UVはz,wが不要項目になるがモーフとしてのデータ値は記録しておく
	//
	//○ボーンモーフ
	//n  : ボーンIndexサイズ  | ボーンIndex
	//12 : float3	| 移動量(x,y,z)
	//16 : float4	| 回転量-クォータニオン(x,y,z,w)
	//
	//○材質モーフ
	//n  : 材質Indexサイズ  | 材質Index -> -1:全材質対象
	//1  : オフセット演算形式 | 0:乗算, 1:加算 - 詳細は後述
	//16 : float4	| Diffuse (R,G,B,A) - 乗算:1.0／加算:0.0 が初期値となる(同以下)
	//12 : float3	| Specular (R,G,B)
	//4  : float	| Specular係数
	//12 : float3	| Ambient (R,G,B)
	//16 : float4	| エッジ色 (R,G,B,A)
	//4  : float	| エッジサイズ
	//16 : float4	| テクスチャ係数 (R,G,B,A)
	//16 : float4	| スフィアテクスチャ係数 (R,G,B,A)
	//16 : float4	| Toonテクスチャ係数 (R,G,B,A)
	//
	//○グループモーフ
	//n  : モーフIndexサイズ  | モーフIndex  ※仕様上グループモーフのグループ化は非対応とする
	//4  : float	| モーフ率 : グループモーフのモーフ値 * モーフ率 = 対象モーフのモーフ値
	//
	//を個々のモーフオフセットとする。
	//
	//
	//[モーフデータ]
	//
	//4 + n : TextBuf	| モーフ名
	//4 + n : TextBuf	| モーフ名英
	//
	//1  : byte	| 操作パネル (PMD:カテゴリ) 1:眉(左下) 2:目(左上) 3:口(右上) 4:その他(右下)  | 0:システム予約
	//
	//1  : byte	| モーフ種類 - 0:グループ, 1:頂点, 2:ボーン, 3:UV, 4:追加UV1, 5:追加UV2, 6:追加UV3, 7:追加UV4, 8:材質
	//
	//4  : int  	| モーフのオフセット数 : 後続の要素数
	//オフセットデータ * n  | モーフ種類に従ってオフセットデータを格納 ※異なる種類の混合は不可

	//	ここはファイルには無し（自作設定）
	int			morph_flag;			//	モーフ実行スイッチ
	float		morph_step;			//	モーフ実行率
	char*		sjis_name;
	
} MMDPI_PMX_MORPH_INFO, *MMDPI_PMX_MORPH_INFO_PTR;

//	表示枠	枠内要素
typedef struct tagMMDPI_PMX_SHOW_FRAME_INFO_INLINE
{

	BYTE		type;
	dword		index;		//	要素対象 0:ボーン 1:モーフ

} MMDPI_PMX_SHOW_FRAME_INFO_INLINE, *MMDPI_PMX_SHOW_FRAME_INFO_INLINE_PTR;

//	表示枠
typedef struct tagMMDPI_PMX_SHOW_FRAME_INFO
{

	char*		name;
	char*		eng_name;

	BYTE		frame_flag;	

	dword		index_num;	//	枠内要素数 : 後続の要素数

	MMDPI_PMX_SHOW_FRAME_INFO_INLINE_PTR	target_index;

	//4 + n : TextBuf	| 枠名
	//4 + n : TextBuf	| 枠名英
	//
	//1  : byte	| 特殊枠フラグ - 0:通常枠 1:特殊枠
	//
	//4  : int  	| 枠内要素数 : 後続の要素数
	//<枠内要素>
	//  1 : byte	| 要素対象 0:ボーン 1:モーフ
	//  ----
	//  要素対象:0の場合
	//    n  : ボーンIndexサイズ  | ボーンIndex
	//
	//  要素対象:1の場合
	//    n  : モーフIndexサイズ  | モーフIndex
	//  ----
	//</枠内要素>
	//* 枠内要素数

} MMDPI_PMX_SHOW_FRAME_INFO, *MMDPI_PMX_SHOW_FRAME_INFO_PTR;

//#pragma pack( pop )	//アラインメント制御エンド
