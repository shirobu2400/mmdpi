

//	mmdpi オプション

#ifdef _WIN32

//#define _MMDPI_OPENGL_ES_DEFINES_			//	OpenGL ES MODE
//#define _MMDPI_PRIJECTION_MATRIX_SELF_		//	透視射影行列のセルフ設定
#define _MMDPI_USING_PHYSICS_ENGINE_			//	物理エンジンをコンパイルから除去
//#define _MMDPI_AMD64BIT_				//	amd x64 コンパイラ環境
//#define _MMDPI_OUTLINE_				//	輪郭処理（法線ベクトルを処理するか？）
//#define _MMDPI_USINGSKIN_				//	表情、モーフを使用

#endif

//	1 マテリアルが所有することのできるボーン数
#define _MMDPI_MATERIAL_USING_BONE_NUM_		24

#define _MMDPI_BULLET_STEP_			2			//	Bullet physics step
#define _MMDPI_BULLET_SPACE_			64			//	Bullet world size
