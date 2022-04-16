
#include "../model/tools/bitmap.hpp"
#include "../model/tools/tga_image.hpp"
#include "../model/mmdpi_struct.hpp"
#include <cstring>
#include <string>


#ifndef		__MMDPI__PMD__STRUCT__DEFINES__
#define		__MMDPI__PMD__STRUCT__DEFINES__	( 1 )


#pragma pack( push, 1 )	//アラインメント制御をオフる

// ヘッダ
typedef struct tagMMDPI_PMD_HEADER
{
	char	modelName[ 32 ];		//モデルネーム
	float	version;			//バージョン
	char	comment[ 256 ];		//コメント　著作権情報
} MMDPI_PMD_HEADER;

// 頂点
typedef struct tagMMDPI_PMD_VERTEX
{
	float	pos[ 3 ];	//x, y, z
	float	nor[ 3 ];	//法線
	float	uv[ 2 ];	//uv
	ushort	bone_num[ 2 ];	// ボーン番号1、番号2 // モデル変形(頂点移動)時に影響
	BYTE	bone_weight;	// ボーン1に与える影響度 // min:0 max:100 // ボーン2への影響度は、(100 - bone_weight)
	BYTE	edge_flag;	// 0:通常、1:エッジ無効 // エッジ(輪郭)が有効の場合
} MMDPI_PMD_VERTEX, *MMDPI_PMD_VERTEX_PTR;

// 面
typedef ushort tagMMDPI_PMD_FACE,
	MMDPI_PMD_FACE, *MMDPI_PMD_FACE_PTR;

/*typedef struct tagMMDPI_PMD_FACE
{
	ushort idx[ 3 ];
} MMDPI_PMD_FACE, *MMDPI_PMD_FACE_PTR;*/

// 材質
typedef struct tagMMDPI_PMD_MATERIAL
{
	float	diffuse_color[ 3 ]; // dr, dg, db // 減衰色
	float	alpha;			// < 1 であると両面描画になる
	float	specularity;
	float	specular_color[ 3 ]; // sr, sg, sb // 光沢色
	float	mirror_color[ 3 ]; // mr, mg, mb // 環境色(ambient)
	BYTE	toon_index; // toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 ・・・ 10.bmp:0x09
	BYTE	edge_flag; // 輪郭、影
	dword	face_vert_count; // 面頂点数 // インデックスに変換する場合は、材質0から順に加算
	char	texture_file_name[ 20 ]; // テクスチャファイル名またはスフィアファイル名 // 20バイトぎりぎりまで使える(終端の0x00は無くても動く)
} MMDPI_PMD_MATERIAL, *MMDPI_PMD_MATERIAL_PTR;

// ボーン
typedef struct tagMMDPI_PMD_BONE_INFO
{
	char	bone_name[ 20 ]; // ボーン名
	ushort	parent_bone_index; // 親ボーン番号(ない場合は0xFFFF)
	ushort	tail_pos_bone_index; // tail位置のボーン番号(チェーン末端の場合は0xFFFF) // 親：子は1：多なので、主に位置決め用
	BYTE	bone_type; // ボーンの種類
	ushort	ik_parent_bone_index; // IKボーン番号(影響IKボーン。ない場合は0)
	float	bone_head_pos[ 3 ]; // x, y, z // ボーンのヘッドの位置

	//・ボーンの種類
	//0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示

	//・ボーンの種類 (MMD 4.0～)
	//8:捻り 9:回転運動
/*
	補足：
	1モデルあたりのボーン数の実質的な限界が変わったようです。(MMD6.x～)
	実験結果：
	MMD6.02、6.08
	約500本x1(テストに使ったデータは513本)→落ちない
	約2000本x1(テストに使ったデータは2599本)→落ちる
	約500本x10→落ちない
	MMD5.22
	約500本x1→落ちない
	約2000本x1→落ちない
*/
} MMDPI_PMD_BONE_INFO, *MMDPI_PMD_BONE_INFO_PTR;

// インバースキネマティクス
typedef struct tagMMDPI_PMD_IK_INFO
{
	ushort		ik_bone_index; // IKボーン番号
	ushort		ik_target_bone_index; // IKターゲットボーン番号 // IKボーンが最初に接続するボーン
	BYTE		ik_chain_length; // IKチェーンの長さ(子の数)
	ushort		iterations; // 再帰演算回数 // IK値1
	float		control_weight; // IKの影響度 // IK値2
	ushort*		ik_child_bone_index; // IK影響下のボーン番号 配列大きさ->ik_chain_length
} MMDPI_PMD_IK_INFO, *MMDPI_PMD_IK_INFO_PTR;

// 表情頂点 // "表情" の内部で使用。
typedef struct tagMMDPI_PMD_SKIN_VERTEX
{
	dword	skin_vert_index; // 表情用の頂点の番号(頂点リストにある番号)
	float	skin_vert_pos[ 3 ]; // x, y, z // 表情用の頂点の座標(頂点自体の座標)
} MMDPI_PMD_SKIN_VERTEX, *MMDPI_PMD_SKIN_VERTEX_PTR;

// 表情
typedef struct tagMMDPI_PMD_SKIN
{

	char	skin_name[ 20 ]; //　表情名
	dword	skin_vert_count; // 表情用の頂点数
	BYTE	skin_type;	 // 表情の種類 // 0：base、1：まゆ、2：目、3：リップ、4：その他

	MMDPI_PMD_SKIN_VERTEX_PTR	vertex;	// 表情用の頂点のデータ(16Bytes/vert)

} MMDPI_PMD_SKIN, *MMDPI_PMD_SKIN_PTR;

// 表情枠用表示リスト
typedef struct tagMMDPI_PMD_SKIN_NUMBER
{
	ushort	skin_index; // 表情番号
} MMDPI_PMD_SKIN_NUMBER, *MMDPI_PMD_SKIN_NUMBER_PTR;

// ボーン枠用枠名リスト
typedef struct tagMMDPI_PMD_BONE_NAME
{
	char	disp_name[ 50 ]; // 表情番号
} MMDPI_PMD_BONE_NAME, *MMDPI_PMD_BONE_NAME_PTR;

// ボーン枠用表示リスト
typedef struct tagMMDPI_PMD_BONE_NUMBER
{
	ushort	bone_index; // 枠用ボーン番号
	BYTE	bone_disp_frame_index; // 表示枠番号
	// センター:00 他の表示枠:01～ // センター枠にセンター(index 0)以外のボーンを指定しても表示されません。
} MMDPI_PMD_BONE_NUMBER, *MMDPI_PMD_BONE_NUMBER_PTR;

// 英語ヘッダ
typedef struct tagMMDPI_PMD_ENGLISH_HEADER
{
	BYTE	english_name_compatibility; // 英名対応(01:英名対応あり)
	char	model_name_eg[ 20 ]; // モデル名(英語)
	char	comment_eg[ 256 ]; // コメント(英語)
} MMDPI_PMD_ENGLISH_HEADER, *MMDPI_PMD_ENGLISH_HEADER_PTR;

// 英語ボーンリスト
typedef struct tagMMDPI_PMD_ENGLISH_BONE_NAME
{
	char	bone_name_en[ 20 ]; // ボーン名(英語)
} MMDPI_PMD_ENGLISH_BONE_NAME, *MMDPI_PMD_ENGLISH_BONE_NAME_PTR;

// 英語表情リスト
typedef struct tagMMDPI_PMD_ENGLISH_SKIN_NAME
{
	char	skin_name_en[ 20 ]; // ボーン名(英語)
} MMDPI_PMD_ENGLISH_SKIN_NAME, *MMDPI_PMD_ENGLISH_SKIN_NAME_PTR;

// 英語ボーン枠用枠名リスト
typedef struct tagMMDPI_PMD_ENGLISH_BONE_DISP_NAME
{
	char	bone_disp_en[ 50 ]; // ボーン名(英語)
} MMDPI_PMD_ENGLISH_BONE_DISP_NAME, *MMDPI_PMD_ENGLISH_BONE_DISP_NAME_PTR;

// toon 情報
typedef struct tagMMDPI_PMD_TOON_NAME
{
	char	toon_file_name[ 10 ][ 100 ];	// トゥーンテクスチャファイル名
} MMDPI_PMD_TOON_NAME, *MMDPI_PMD_TOON_NAME_PTR;

// 物理演算 // 剛体情報
typedef struct tagMMDPI_PMD_PHYSICAL_OBJECT
{
	char	rigidbody_name[ 20 ]; // 諸データ：名称 // 頭
	ushort	rigidbody_rel_bone_index; // 諸データ：関連ボーン番号 // 03 00 == 3 // 頭
	BYTE	rigidbody_group_index; // 諸データ：グループ // 00
	ushort	rigidbody_group_mask; // 諸データ：グループ：対象 // 0xFFFFとの差 // 38 FE
	BYTE	shape_type; // 形状：タイプ(0:球、1:箱、2:カプセル) // 00 // 球
	float	shape_w; // 形状：半径(幅) // CD CC CC 3F // 1.6
	float	shape_h; // 形状：高さ // CD CC CC 3D // 0.1
	float	shape_d; // 形状：奥行 // CD CC CC 3D // 0.1
	float	pos_pos[ 3 ]; // 位置：位置(x, y, z)
	float	pos_rot[ 3 ]; // 位置：回転(rad(x), rad(y), rad(z))
	float	rigidbody_weight; // 諸データ：質量 // 00 00 80 3F // 1.0
	float	rigidbody_pos_dim; // 諸データ：移動減 // 00 00 00 00
	float	rigidbody_rot_dim; // 諸データ：回転減 // 00 00 00 00
	float	rigidbody_recoil; // 諸データ：反発力 // 00 00 00 00
	float	rigidbody_friction; // 諸データ：摩擦力 // 00 00 00 00
	BYTE	rigidbody_type; // 諸データ：タイプ(0:Bone追従、1:物理演算、2:物理演算(Bone位置合せ)) // 00 // Bone追従
} MMDPI_PMD_PHYSICAL_OBJECT, *MMDPI_PMD_PHYSICAL_OBJECT_PTR;

// 物理演算 // ジョイント情報
typedef struct tagMMDPI_PMD_PHYSICAL_JOINT
{
	char	joint_name[ 20 ]; // 諸データ：名称 // 右髪1
	dword	joint_rigidbody_a; // 諸データ：剛体A
	dword	joint_rigidbody_b; // 諸データ：剛体B
	float	joint_pos[ 3 ]; // 諸データ：位置(x, y, z) // 諸データ：位置合せでも設定可
	float	joint_rot[ 3 ]; // 諸データ：回転(rad(x), rad(y), rad(z))
	float	constrain_pos_1[ 3 ]; // 制限：移動1(x, y, z)
	float	constrain_pos_2[ 3 ]; // 制限：移動2(x, y, z)
	float	constrain_rot_1[ 3 ]; // 制限：回転1(rad(x), rad(y), rad(z))
	float	constrain_rot_2[ 3 ]; // 制限：回転2(rad(x), rad(y), rad(z))
	float	spring_pos[ 3 ]; // ばね：移動(x, y, z)
	float	spring_rot[ 3 ]; // ばね：回転(rad(x), rad(y), rad(z))
} MMDPI_PMD_PHYSICAL_JOINT, *MMDPI_PMD_PHYSICAL_JOINT_PTR;


#pragma pack( pop )	//アラインメント制御エンド

#endif	//	__MMDPI__PMD__STRUCT__DEFINES__
