#	cmake 必要バージョン
cmake_minimum_required(VERSION 2.6)

#	プロジェクト名	と 言語(C=C, CXX=C++)
project(mmdpi CXX)

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
	bitmap.cpp        pmx_draw.h        pmd_draw.h       pmx_ik.cpp
	bitmap.h          mmdpi_include.h   pmd_ik.cpp       pmx_ik.h
	get_bin.cpp       mmdpi_model.cpp   pmd_ik.h         pmx_load.cpp
	get_bin.h         mmdpi_model.h     pmd_load.cpp     pmx_load.h
	mmdpi.cpp         mmdpi_shader.cpp  pmd_load.h       pmx_struct.h
	mmdpi.h           mmdpi_shader.h    pmd_struct.h     png.cpp
	mmdpi_adjust.cpp  mmdpi_struct.h    pmx.cpp          tga_image.cpp
	mmdpi_adjust.h    pmd.cpp           pmx.h            tga_image.h
	mmdpi_bone.cpp    pmd.h             pmx_analyze.cpp  utf8tosjis.h
	mmdpi_bone.h      pmd_analyze.cpp   pmx_analyze.h    vmd.cpp
	pmd_analyze.h     pmx_draw.cpp     	vmd.h            pmd_draw.cpp

)

#	Include Directory
set(libmmdpi_inc
	/usr/local/include/
	./GL 
	./$(LIBPNG)
)

if(WIN32)
	set(libmmdpi_inc ${libmmdpi_inc} define)
else(pi)
	set(libmmdpi_inc ${libmmdpi_inc} define_es)
else()
	set(libmmdpi_inc ${libmmdpi_inc} define_unix)
endif()

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

#	Bullet
#set(${libmmdpi_src} ${libmmdpi_src} ${libmmdpi_bullet_src})

#	Include
set(libmmdpi_bullet_inc
	-I./bullet/
)


#	静的ライブラリとしてビルド
add_library(mmdpi STATIC ${libmmdpi_src})
include_directories( ${libmmdpi_inc} )
include_directories( ${libmmdpi_inc_raspi} )
include_directories( ${libmmdpi_bullet_inc} )
