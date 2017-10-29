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
	model/tools/get_bin.cpp
	model/tools/bitmap.cpp
	model/tools/png.cpp
	model/tools/jpg.cpp
	model/tools/tga_image.cpp

	model/mmdpi_model.cpp
	model/mmdpi_shader.cpp
	model/mmdpi_adjust.cpp
	model/mmdpi_bone.cpp

	pmx/pmx_ik.cpp
	pmx/pmx_load.cpp
	pmx/pmx_analyze.cpp
	pmx/pmx_draw.cpp
	pmx/pmx.cpp

	pmd/pmd_ik.cpp
	pmd/pmd_load.cpp
	pmd/pmd_analyze.cpp
	pmd/pmd_draw.cpp
	pmd/pmd.cpp

	vmd/vmd.cpp
)

# header
set(libmmdpi_header
	mmdpi.h

	model/mmdpi_bone.h
	model/mmdpi_adjust.h
	model/mmdpi_define.h
	model/mmdpi_include.h
	model/mmdpi_model.h
	model/mmdpi_shader.h
	model/mmdpi_struct.h

	pmd/pmd.h
	pmd/pmd_analyze.h
	pmd/pmd_draw.h
	pmd/pmd_ik.h
	pmd/pmd_load.h
	pmd/pmd_struct.h

	pmx/pmx.h
	pmx/pmx_analyze.h
	pmx/pmx_draw.h
	pmx/pmx_ik.h
	pmx/pmx_load.h
	pmx/pmx_struct.h

	vmd/vmd.h
)


#	Include Directory
set(libmmdpi_inc
	/usr/local/include/
	/usr/include/bullet/
	/usr/include/bullet/BulletDynamics/
	/usr/include/bullet/BulletSoftBody/
	/usr/include/bullet/LinearMath/
	/usr/include/bullet/BulletCollision/
)

#	Raspberry Pi
set(libmmdpi_inc_raspi
	/opt/vc/include/interface/vmcs_host/linux
	/opt/vc/include
	/opt/vc/include/interface/vcos/pthreads
)

#	Bullet Physics Source
set(libmmdpi_bullet_src
	model/bullet/mmdpi_bullet.cpp
	model/bullet/mmdpi_bullet.h
)

find_package( PNG REQUIRED )
find_package( JPEG REQUIRED )
find_package( GLUT )
find_package( glut )
find_package( freeglut )

#	OpenGL
if( NOT GLUT_FOUND AND NOT glut_FOUND AND NOT freeglut_FOUND )
	add_definitions( -D_MMDPI_OPENGL_ES_DEFINES_ )
	add_definitions( -D_MMDPI_PRIJECTION_MATRIX_SELF_ )
endif()

##	Bullet
#find_package( Bullet )
#if( bullet AND Bullet_FOUND )
if( bullet )
	set( libmmdpi_src ${libmmdpi_src} ${libmmdpi_bullet_src} )

	add_definitions( -D_MMDPI_USING_PHYSICS_ENGINE_ )
	include_directories( Bullet_INCLUDE_DIRS /usr/local/include/bullet )
endif()

#	Outline
#	It needs a lot of VRAM.
if( outline )
	add_definitions( -D_MMDPI_OUTLINE_ )
endif()

if( x64 )
	add_definitions( -D_MMDPI_AMD64BIT_ )
else()
	set( CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} -m32 )
endif()

# 静的ライブラリとしてビルド
add_library(mmdpi STATIC ${libmmdpi_src})
include_directories( ${libmmdpi_inc} )
include_directories( ${libmmdpi_inc_raspi} )

# 動的ライブラリも作っておく python で使えるようにするつもり
add_library(mmdpi SHARED ${libmmdpi_src})

# make install
install(TARGETS mmdpi LIBRARY DESTINATION lib/mmdpi)
install(FILES ${libmmdpi_header} DESTINATION include/mmdpi)
