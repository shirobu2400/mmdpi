#	cmake 必要バージョン
cmake_minimum_required(VERSION 2.6)

#	プロジェクト名	と 言語(C=C, CXX=C++)
project(mmdpi C CXX)

#	リリースオプション
set(CMAKE_CXX_FLAG_RELEASE "-Wall -Os")

#	デバックオプション
set(CMAKE_CXX_FLAG_DEBUG "-g")

#	コンパイルオプション
set(CMAKE_BUILD_TYPE Release)

#	バージョン
set(SERIAL "1.00.00")

#	Source
set(libmmdpi_src
	mmdpi.cpp         
	model/tools/cpp/get_bin.cpp       
	model/tools/cpp/bitmap.cpp        
	model/tools/cpp/png.cpp
	model/tools/cpp/tga_image.cpp
	
	model/cpp/mmdpi_model.cpp   
	model/cpp/mmdpi_shader.cpp  
	model/cpp/mmdpi_adjust.cpp  
	model/cpp/mmdpi_bone.cpp    
	
	pmx/cpp/pmx_ik.cpp
	pmx/cpp/pmx_load.cpp
	pmx/cpp/pmx_analyze.cpp  
	pmx/cpp/pmx_draw.cpp
	pmx/cpp/pmx.cpp          
	
	pmd/cpp/pmd_ik.cpp       
	pmd/cpp/pmd_load.cpp
	pmd/cpp/pmd_analyze.cpp   
	pmd/cpp/pmd_draw.cpp
	pmd/cpp/pmd.cpp           
	
	vmd/vmd.cpp
)

#	Include Directory
set(libmmdpi_inc
	/usr/local/include/
	./GL 
	./$(LIBPNG)
)

#	Raspberry Pi
set(libmmdpi_inc_raspi
	/opt/vc/include/interface/vmcs_host/linux 
	/opt/vc/include 
	/opt/vc/include/interface/vcos/pthreads
)

#	Bullet Physics Source
set(libmmdpi_bullet_src
	mmdpi_bullet.cpp
	mmdpi_bullet.h 
)

#	OpenGL ES
if( ES OR pi )
	add_definitions( -D_MMDPI_OPENGL_ES_DEFINES_ )
	add_definitions( -D_MMDPI_PRIJECTION_MATRIX_SELF_ )
endif()

#	Bullet
if( bullet )
	set(${libmmdpi_src} ${libmmdpi_src} ${libmmdpi_bullet_src})
	add_definitions( -D_MMDPI_USING_PHYSICS_ENGINE_ )
endif()

#	Outline
#	It needs a lot of VRAM.
if( outline )
	add_definitions( -D_MMDPI_OUTLINE_ )
endif()

#	Include
set(libmmdpi_bullet_inc
	-I./bullet/
)

#	静的ライブラリとしてビルド
add_library(mmdpi STATIC ${libmmdpi_src})
include_directories( ${libmmdpi_inc} )
include_directories( ${libmmdpi_inc_raspi} )
include_directories( ${libmmdpi_bullet_inc} )