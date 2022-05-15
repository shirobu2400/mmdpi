
#include "mmdpi_include.hpp"

#include "tools/bitmap.hpp"
#include "tools/tga_image.hpp"
#include "jpeglib.h"
#include "png.h"
#include "tools/dds.hpp"

#ifndef		__MMDPI__STRUCT__DEFINES__
#define		__MMDPI__STRUCT__DEFINES__	( 1 )

typedef		ushort	mmdpiShaderIndex;
//typedef		uint	mmdpiShaderIndex;

//#pragma pack( push, 1 )	//アラインメント制御をオフる

//	頂点データ型
typedef struct tagMMDPI_VERTEX
{
	//	頂点
	mmdpiVector3d	ver;

	//	テクスチャ座標
	mmdpiVector4d	uv;

	//	ボーンインデックス
	mmdpiVector4d	index;

	//	ボーン重み
	mmdpiVector4d	weight;

	//	法線ベクトル
	mmdpiVector3d	nor;

} MMDPI_VERTEX, *MMDPI_VERTEX_PTR;

//#pragma pack( pop )	//アラインメント制御エンド

// 頂点集合データ型
typedef struct tagMMDPI_BLOCK_VERTEX
{
	//	各変数は個数が同期している。
	//	個数を格納
	dword			num;

	//	頂点群
	mmdpiVector3d_ptr	ver;

	//	テクスチャ座標群
	mmdpiVector4d_ptr	uv;

	//	ボーンインデックス群
	mmdpiVector4d_ptr	index;

	//	ボーン重み群
	mmdpiVector4d_ptr	weight;

	//	法線ベクトル群
	mmdpiVector3d_ptr	nor;

	//	メモリ自動開放フラグ
	int			alloc_flag;

	tagMMDPI_BLOCK_VERTEX()
	{
		this->num = 0;
		ver	= 0;
		uv	= 0;
		nor	= 0;
		index	= 0;
		weight	= 0;
		alloc_flag = 0;
	}
	void alloc( dword num )
	{
		this->num = num;
		ver	= new mmdpiVector3d[ num ];
		uv	= new mmdpiVector4d[ num ];
		nor	= new mmdpiVector3d[ num ];
		index	= new mmdpiVector4d[ num ];
		weight	= new mmdpiVector4d[ num ];
		alloc_flag = 1;
	}
	~tagMMDPI_BLOCK_VERTEX()
	{
		if( alloc_flag == 0 )
			return ;
		delete[] ver;
		delete[] uv;
		delete[] nor;
		delete[] index;
		delete[] weight;
	}

} MMDPI_BLOCK_VERTEX, *MMDPI_BLOCK_VERTEX_PTR;

//	pnglib
struct MMDPI_PNG
{
	//pngInfo 		info;
	GLuint			texture;

	GLuint get_id( void )
	{
		return texture;
	}
	GLuint load( const char *filename );

	MMDPI_PNG()
	{
		texture = 0;
	}
	~MMDPI_PNG()
	{
		glDeleteTextures( 1, &texture );
	}
};

// jpeg lib
struct MMDPI_JPG
{
	GLuint			texture;

	GLuint get_id( void )
	{
		return texture;
	}
	GLuint load( const char *filename );

	MMDPI_JPG()
	{
		texture = 0;
	}
	~MMDPI_JPG()
	{
		glDeleteTextures( 1, &texture );
	}
};

// イメージ管理
typedef struct tagMMDPI_IMAGE
{
	int		type;		// 0->bmp, 1->tga
	int		ref;		// 参照しているか

	MMDPI_BMP*	bmp;
	MMDPI_TGA*	tga;
	MMDPI_PNG*	png;
	MMDPI_JPG*	jpg;
	MMDPI_DDS*	dds;

	GLint		id;

	int load( const char *file_name )
	{
		uint	len = strlen( file_name );
		ref = 0;

		if( strncmp( file_name + len - 4, ".tga", 4 ) == 0 || strncmp( file_name + len - 4, ".TGA", 4 ) == 0 )
		{
			tga = new MMDPI_TGA();
			if( tga && ( int )tga->load( file_name ) < 0 )
			{
				delete tga;
				return -1;
			}
			type = 1;
			id = tga->get_id();
		}
		else if( strncmp( file_name + len - 4, ".png", 4 ) == 0 || strncmp( file_name + len - 4, ".PNG", 4 ) == 0 )
		{
			png = new MMDPI_PNG();
			if( png && ( int )png->load( file_name ) < 0 )
			{
				delete png;
				return -1;
			}
			type = 2;
			id = png->get_id();
		}
		else if( strncmp( file_name + len - 4, ".bmp", 4 ) == 0 || strncmp( file_name + len - 4, ".BMP", 4 ) == 0 )
		{
			bmp = new MMDPI_BMP();
			if( bmp && ( int )bmp->load( file_name ) < 0 )
			{
				delete bmp;
				return -1;
			}
			type = 3;
			id = bmp->get_id();
		}
		else if( strncmp( file_name + len - 4, ".jpg", 4 ) == 0 || strncmp( file_name + len - 5, ".jpeg", 5 ) == 0 ||
			strncmp( file_name + len - 4, ".JPG", 4 ) == 0 || strncmp( file_name + len - 5, ".JPEG", 5 ) == 0 )
		{
			jpg = new MMDPI_JPG();
			if( jpg && ( int )jpg->load( file_name ) < 0 )
			{
				delete jpg;
				return -1;
			}
			type = 4;
			id = jpg->get_id();
		}
		else if( strncmp( file_name + len - 4, ".dds", 4 ) == 0 || strncmp( file_name + len - 4, ".DDS", 4 ) == 0 )
		{
			dds = new MMDPI_DDS();
			if( dds && ( int )dds->load( file_name ) < 0 )
			{
				delete dds;
				return -1;
			}
			type = 5;
			id = dds->get_id();
		}

		return type;
	}

	GLuint copy( const tagMMDPI_IMAGE& c )
	{
		if( c.id < 0 )
			return -1;
		type = c.type;
		ref = 1;
		return id = c.id;
	}

	int get_id( void )
	{
		return id;
	}

	tagMMDPI_IMAGE()
	{
		id = -1;
		type = 0;
		ref = 0;

		tga = 0x00;
		png = 0x00;
		bmp = 0x00;
		jpg = 0x00;
		dds = 0x00;
	}

	~tagMMDPI_IMAGE()
	{
		if( tga )
		{
			delete tga;
			tga = 0x00;
		}
		if( png )
		{
			delete png;
			png = 0x00;
		}
		if( bmp )
		{
			delete bmp;
			bmp = 0x00;
		}
		if( jpg )
		{
			delete jpg;
			jpg = 0x00;
		}
		if( dds )
		{
			delete dds;
			dds = 0x00;
		}
	}

} MMDPI_IMAGE, *MMDPI_IMAGE_PTR;

//	材質情報データ
typedef struct tagMMDPI_MATERIAL
{
	dword				face_top;
	dword				face_num;
	MMDPI_IMAGE			texture;
	MMDPI_IMAGE			toon_texture;
} MMDPI_MATERIAL, *MMDPI_MATERIAL_PTR;

//	メッシュのデータ群
typedef struct tagMMDPI_PIECE
{
	dword				face_top;
	dword				face_num;

	int				raw_material_id;
	MMDPI_MATERIAL*			raw_material;

	dword				bone_list_num;
	dword				bone_list[ _MMDPI_MATERIAL_USING_BONE_NUM_ ];

	mmdpiMatrix			matrix[ _MMDPI_MATERIAL_USING_BONE_NUM_ ];

	mmdpiColor			color;
	float				edge_size;
	mmdpiColor			edge_color;
	float				opacity;

	int				has_texture;

	// 両面描画フラグ
	int				is_draw_both;

} MMDPI_PIECE;

typedef struct tagMMDPI_BONE_INFO
{
	int					id;			// ボーンID（通し番号）

	char*					name;
	char*					sjis_name;

	int					level;

	int					visible;		// 表示するか否か
	float					length;			// 表示用ボーンの長さ

	tagMMDPI_BONE_INFO*			parent;			// 親ボーン

	tagMMDPI_BONE_INFO*			first_child;		// 第1子ボーン
	tagMMDPI_BONE_INFO*			sibling;		// 次の兄弟ボーン

	int					child_flag;		// 0:座標オフセットで指定 1:ボーンで指定
	tagMMDPI_BONE_INFO*			child_bone;
	mmdpiMatrix				posoffset_matrix;	// child_flag == 0 の時のみ


	mmdpiMatrix				init_matrix;		// 親ボーンから見た初期姿勢行列
	mmdpiMatrix				offset_matrix;		// 親ボーンから見たボーンオフセット行列
	mmdpiMatrix				temp_matrix;		// ボーン姿勢行列 => 親ボーンから見た姿勢行列 global matrix の一時保存機能
	mmdpiMatrix				local_matrix;		// シェーダー上の頂点は「すべての親ボーン」を基準に設計されているため、offset_matrix * global_matrix になる
	mmdpiMatrix				global_matrix;		// offsetMat に matrix を掛けた状態(実際にシェーダに渡す値) 座標系に対する行列
	mmdpiMatrix				delta_matrix;		// 変化量行列（ユーザー定義）

	tagMMDPI_BONE_INFO()
	{
		name = 0;
		sjis_name = 0;
		level = 0;
	}

	~tagMMDPI_BONE_INFO()
	{
		delete[] name;
		delete[] sjis_name;
	}

} MMDPI_BONE_INFO, *MMDPI_BONE_INFO_PTR;

// Skin
typedef struct tagMMDPI_SKIN_INFO
{
	dword	skin_flag;
	char*	skin_name;
} MMDPI_SKIN_INFO, *MMDPI_SKIN_INFO_PTR;


// 剛体
typedef struct tagMMDPI_PHYSICAL_RIGID_INFO
{
	char*		name;
	char*		eng_name;

	dword		bone_index;	// 関連ボーンIndex - 関連なしの場合は-1

	BYTE		group;
	ushort		not_touch_group_flag;

	BYTE		type;		// 形状 - 0:球 1:箱 2:カプセル

	float		size[ 3 ];

	float		pos[ 3 ];
	float		rot[ 3 ];

	float		mass;		// 質量
	float		ac_t;		// 移動減衰
	float		ac_r;		// 回転減衰
	float		repulsion;	// 反発力
	float		friction;	// 摩擦力

	BYTE		rigidbody_type;	// 剛体の物理演算 -
	// 0:ボーン追従(static)		ボーンを剛体に合わせる
	// 1:物理演算(dynamic)		物理演算のみを行い、ボーンは合わせない
	// 2:物理演算 + Bone位置合わせ

	//4 + n : TextBuf	| 剛体名
	//4 + n : TextBuf	| 剛体名英
	//
	//n  : ボーンIndexサイズ  | 関連ボーンIndex - 関連なしの場合は-1
	//
	//1  : byte	| グループ
	//2  : ushort	| 非衝突グループフラグ
	//
	//1  : byte	| 形状 - 0:球 1:箱 2:カプセル
	//12 : float3	| サイズ(x,y,z)
	//
	//12 : float3	| 位置(x,y,z)
	//12 : float3	| 回転(x,y,z) -> ラジアン角
	//
	//4  : float	| 質量
	//4  : float	| 移動減衰
	//4  : float	| 回転減衰
	//4  : float	| 反発力
	//4  : float	| 摩擦力
	//
	//1  : byte	| 剛体の物理演算 - 0:ボーン追従(static) 1:物理演算(dynamic) 2:物理演算 + Bone位置合わせ

} MMDPI_PHYSICAL_RIGID_INFO, *MMDPI_PHYSICAL_RIGID_INFO_PTR;

//	ジョイント
typedef struct tagMMDPI_PHYSICAL_JOINT_INFO
{
	char*		name;
	char*		eng_name;

	BYTE		type;

	dword		a_index, b_index;

	float		pos[ 3 ];
	float		rot[ 3 ];

	float		trans_limit1[ 3 ];
	float		trans_limit2[ 3 ];

	float		rotate_limit1[ 3 ];
	float		rotate_limit2[ 3 ];

	float		spring_trans[ 3 ];
	float		spring_rotate[ 3 ];

	//4 + n : TextBuf	| Joint名
	//4 + n : TextBuf	| Joint名英

	//1  : byte	| Joint種類 - 0:スプリング6DOF   | PMX2.0では 0 のみ(拡張用)
	//--
	//Joint種類:0 の場合
	//
	//n  : 剛体Indexサイズ  | 関連剛体AのIndex - 関連なしの場合は-1
	//n  : 剛体Indexサイズ  | 関連剛体BのIndex - 関連なしの場合は-1
	//
	//12 : float3	| 位置(x,y,z)
	//12 : float3	| 回転(x,y,z) -> ラジアン角
	//
	//12 : float3	| 移動制限-下限(x,y,z)
	//12 : float3	| 移動制限-上限(x,y,z)
	//12 : float3	| 回転制限-下限(x,y,z) -> ラジアン角
	//12 : float3	| 回転制限-上限(x,y,z) -> ラジアン角
	//
	//12 : float3	| バネ定数-移動(x,y,z)
	//12 : float3	| バネ定数-回転(x,y,z)
	//--

} MMDPI_PHYSICAL_JOINT_INFO, *MMDPI_PHYSICAL_JOINT_INFO_PTR;

#endif	//	__MMDPI__STRUCT__DEFINES__
