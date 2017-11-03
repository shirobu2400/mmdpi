

#include "mmdpi_shader.h"
#include "mmdpi_adjust.h"
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
#	include "bullet/mmdpi_bullet.h"
#endif

#pragma once

class mmdpiBone : public mmdpiShader, public mmdpiAdjust
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	, public mmdpiBullet
#endif	
{

protected :

	uint				bone_num;				//	ボーン数 ( ushort でＯＫ )
	MMDPI_BONE_INFO_PTR		bone;					//	ボーン

	//	姿勢行列合成用
	mmdpiMatrix*			bone_comb_mat;

	void				init_mat_calc( MMDPI_BONE_INFO_PTR my_bone, mmdpiMatrix* offset );
	mmdpiMatrix			init_mat_calc_bottom( MMDPI_BONE_INFO_PTR now_bone );

	//	物理演算用
	int				bullet_flag;				//	物理演算実行
	int				phy_load_flag;				// 物理演算を読み込む
	dword				rigidbody_count;			// 剛体数
	MMDPI_PHYSICAL_RIGID_INFO_PTR	physics;				// 物理演算	// 剛体
#ifdef _MMDPI_USING_PHYSICS_ENGINE_
	MMDPI_PHYSICAL_INFO_PTR		physics_sys;				// 物理演算作業領域
#endif
	dword				joint_count;				// ジョイント数
	MMDPI_PHYSICAL_JOINT_INFO_PTR	joint;					// 物理演算 // ジョイント

	int				create_physical_info( void );

public :
	
	static mmdpiMatrix	get_global_matrix( MMDPI_BONE_INFO_PTR bone );
	
	void			refresh_bone_mat( void );
	void			make_local_matrix( void );

	int			set_bone_matrix( uint bone_index, const mmdpiMatrix& matrix );

	int			make_matrix( MMDPI_BONE_INFO_PTR my_bone, const mmdpiMatrix* offset );
	int			global_matrix( void );
	mmdpiMatrix		make_global_matrix( int index );
	
	int			advance_time_physical( int fps = 30 );
	
	dword get_bone_num( void )
	{ 
		return bone_num;
	}
	MMDPI_BONE_INFO_PTR get_bone( void )
	{
		return bone;
	}

//	Bullet

	// 物理演算によるボーン更新
	int					update_bone_physical( dword rigid_id );

	// ボーンに剛体をあわせる
	int					move_to_bone_pos( dword rigid_id );

	// ボーンあわせ
	int					fix_position( dword rigid_id, float fElapsedFrame );
	
	int					set_bullet_flag( int flag ){ return bullet_flag = flag; }


	mmdpiBone();
	~mmdpiBone();
} ;