
#include "../model/mmdpi_include.h"
#include "../model/mmdpi_bone.h"
#include "../model/tools/get_bin.h"
#include "../pmx/pmx_struct.h"
#include "../pmd/pmd_struct.h"

#pragma once
#pragma pack( push, 1 )		//	アライメント

//	動作
typedef struct tagMMDPI_VMD_MMDPI_PMD_HEADER 
{
	char			VmdHeader[ 30 ];	//	"Vocaloid Motion Data 0002"
	char			VmdModelName[ 20 ];
} MMDPI_VMD_MMDPI_PMD_HEADER;

typedef struct tagMMDPI_VMD_MOTION 
{ // 111 Bytes
	char			BoneName[ 15 ];
	dword			FrameNo;
	float			vx, vy, vz;
	float			qx, qy, qz, qw;			// Quaternion
	char			Interpolation[ 64 ];	// [4][4][4]
} MMDPI_VMD_MOTION, *MMDPI_VMD_MOTION_PTR;

//	表情
typedef struct tagMMDPI_VMD_SKIN_COUNT 
{
	dword						Count;
} MMDPI_VMD_SKIN_COUNT, *MMDPI_VMD_SKIN_COUNT_PTR;

typedef struct tagMMDPI_VMD_SKIN 
{ // 23 Bytes	
	char						SkinName[ 15 ];
	dword						FrameNo;
	float						Weight;
} MMDPI_VMD_SKIN, *MMDPI_VMD_SKIN_PTR;
 
typedef struct tagMMDPI_VMD_INFO
{
	MMDPI_VMD_MMDPI_PMD_HEADER	header;		//	ヘッダ
	
	//	モーション
	dword						motion_num;	//	モーション数
	MMDPI_VMD_MOTION_PTR		motion;		//	モーション

	//	スキン
	MMDPI_VMD_SKIN_COUNT		skin_num;
	MMDPI_VMD_SKIN_PTR			skin;			

} MMDPI_VMD_INFO, *MMDPI_VMD_INFO_PTR;

#if 0
 struct VMD_CAMERA_COUNT {
 dword Count;
 } vmd_camera_count;

 struct VMD_CAMERA { 
dword FlameNo;
 float Length; 
float Location[3];
 float Rotation[3]; 
BYTE Interpolation[24]; 
dword ViewingAngle;
 BYTE Perspective; // 0:on 1:off
 } vmd_camera;

 struct VMD_LIGHT_COUNT {
 dword Count;
 } vmd_light_count;

 struct VMD_LIGHT { // 28 Bytes // 
dword FlameNo;
 float RGB[3]; // RGB
 float Location[3];
 } vmd_light;

 struct VMD_SELF_SHADOW_COUNT {
 dword Count;
 } vmd_self_shadow_count;

 struct VMD_SELF_SHADOW { // 9 Bytes //
dword FlameNo;
 BYTE Mode; // 00-02
 float Distance; // 0.1 - (dist * 0.00001)
 } vmd_self_shadow;
#endif

#pragma pack( pop )	// 

 //	内部処理用
 typedef struct tagMMDPI_VMD_INFO_LIST
 {
	tagMMDPI_VMD_INFO_LIST*		next;		//	
	tagMMDPI_VMD_INFO_LIST*		prev;		//	
	MMDPI_VMD_MOTION_PTR		motion;		//	モーション
 } MMDPI_VMD_INFO_LIST, *MMDPI_VMD_INFO_LIST_PTR, **MMDPI_VMD_INFO_LIST_PTR_PTR;

  //	内部処理用
 typedef struct tagMMDPI_VMD_SKIN_INFO_LIST
 {
	tagMMDPI_VMD_SKIN_INFO_LIST*		next;		//	次
	tagMMDPI_VMD_SKIN_INFO_LIST*		prev;		//	前
	tagMMDPI_VMD_SKIN_INFO_LIST*		brot;		//	兄弟

 	MMDPI_VMD_SKIN_PTR					skin;		//	スキン
	dword								target_id;
 } MMDPI_VMD_SKIN_INFO_LIST, *MMDPI_VMD_SKIN_INFO_LIST_PTR;

 //	VMD Loader
class mmdpiVmd
{
protected :

	MMDPI_VMD_INFO_PTR		vmd_info;
	int				bone_num;

	MMDPI_VMD_INFO_LIST_PTR		m_list;
	map< string, int >		bone_name_to_index;

	//	モーション
	MMDPI_VMD_INFO_LIST_PTR_PTR	motion_line;	//	モーション配列
	MMDPI_VMD_INFO_LIST_PTR_PTR	now_motion;		//	現在のモーション

	//	表情
	map< string, int >		skin_name_to_index;
	MMDPI_VMD_SKIN_INFO_LIST_PTR	skin_line;
	MMDPI_VMD_SKIN_INFO_LIST_PTR	now_skin;		//	現在のスキン
	
	int				skin_morph_num;
	//MMDPI_PMD_SKIN_INFO_PTR	skin;
	MMDPI_PMX_MORPH_INFO_PTR	morph;


	dword				max_frame;					
	float				motion_time;

	int				analyze( void );
	int				insert_motion_list( int bone_index, MMDPI_VMD_MOTION_PTR insert_motion );
	int				insert_skin( MMDPI_VMD_SKIN_PTR skin );

	float				interpolate( float x1, float y1, float x2, float y2, float x );

	MMDPI_BONE_INFO_PTR		bone;
	
public :
	
	int operator++( void )
	{ 
		return this->advance_time();
	}
	int operator+=( float frame )
	{ 
		if( frame < 1e-4f )
			return 0;
		return this->advance_time( frame );
	}
	mmdpiVmd* operator=( float frame )
	{
		if( motion_time < frame ) 
			this->init_motion();
		for( ; motion_time < frame; motion_time ++ )
			this->advance_time();
		this->advance_time( 0 );
		return this;
	}

	float get_motion_time( void )
	{
		return motion_time;
	}

	float get_motion_frames( void )
	{
		return ( float )max_frame;
	}

	int					is_end( void );					//	終了判定
	int					init_motion( void );				//	モーション初期化
	int					advance_time( float time_scale = 1.0f );	//	モーション再生

	int					set_bone( MMDPI_BONE_INFO_PTR bone );
	int					create_bone_map( MMDPI_PMD_BONE_INFO_PTR bone, int bone_num );
	//int					create_morph_map( MMDPI_PMD_SKIN_INFO_PTR skin, int skin_num );
	int					create_bone_map_pmx( MMDPI_PMX_BONE_INFO_PTR bone, int bone_num );
	int					create_morph_map_pmx( MMDPI_PMX_MORPH_INFO_PTR morph, int morph_num );

	int					init_first_motion( void );
	int					load( const char *filename );

	mmdpiVmd();
	~mmdpiVmd();
} ; 
