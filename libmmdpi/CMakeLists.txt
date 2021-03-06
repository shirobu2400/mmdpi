# cmake version
cmake_minimum_required(VERSION 2.6)

# project name and language option(C=C, CXX=C++)
project(mmdpi C CXX)

# Release options
set(CMAKE_CXX_FLAG_RELEASE "-O2")

# Debug options
# set(CMAKE_CXX_FLAG_DEBUG "-g")

# compile option
set(CMAKE_BUILD_TYPE Release)

# this version
set(SERIAL "1.00.00")

# sources
set(libmmdpi_src
	mmdpi.cpp

	model/tools/get_bin.cpp
	model/tools/bitmap.cpp
	model/tools/png.cpp
	model/tools/jpg.cpp
	model/tools/dds.cpp
	model/tools/tga_image.cpp
	model/tools/utf8_sjis.cpp

	model/mmdpi_model.cpp
	model/mmdpi_shader.cpp
	model/mmdpi_adjust.cpp
	model/mmdpi_bone.cpp
	model/mmdpi_mesh.cpp

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

# headers
set(libmmdpi_header
	mmdpi.h

	model/mmdpi_bone.h
	model/mmdpi_adjust.h
	model/mmdpi_define.h
	model/mmdpi_include.h
	model/mmdpi_model.h
	model/mmdpi_shader.h
	model/mmdpi_struct.h
	model/mmdpi_mesh.h

	model/tools/bitmap.h
	model/tools/get_bin.h
	model/tools/raw_table.h
	model/tools/dds.h
	model/tools/tga_image.h
	model/tools/utf8_sjis.h

	model/bullet/mmdpi_bullet.h

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


# Include Directory
set(libmmdpi_inc
	/usr/local/include
	/usr/include/bullet
	/usr/include/bullet/BulletDynamics
	/usr/include/bullet/BulletSoftBody
	/usr/include/bullet/LinearMath
	/usr/include/bullet/BulletCollision
)

# Raspberry Pi
set(libmmdpi_inc_raspi
	/opt/vc/include/interface/vmcs_host/linux
	/opt/vc/include
	/opt/vc/include/interface/vcos/pthreads
)

# Bullet Physics Source
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

## Bullet
#find_package( Bullet )
#if( bullet AND Bullet_FOUND )
if( bullet )
	set( libmmdpi_src ${libmmdpi_src} ${libmmdpi_bullet_src} )

	add_definitions( -D_MMDPI_USING_PHYSICS_ENGINE_ )
	include_directories( Bullet_INCLUDE_DIRS /usr/local/include/bullet )
endif()

# Outline
# It needs a lot of VRAM.
if( outline )
	add_definitions( -D_MMDPI_OUTLINE_ )
endif()

set( CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} -m32 )

# static library
add_library(mmdpi STATIC ${libmmdpi_src})
include_directories( ${libmmdpi_inc} )
include_directories( ${libmmdpi_inc_raspi} )

# dynamic library
add_library(mmdpis SHARED ${libmmdpi_src})
set_property(TARGET mmdpis PROPERTY POSITION_INDEPENDENT_CODE ON)

# make install
install(TARGETS mmdpi ARCHIVE DESTINATION lib/mmdpi)
install(TARGETS mmdpis LIBRARY DESTINATION lib/mmdpi)

install(FILES mmdpi.h DESTINATION include/mmdpi)
install(FILES
	model/mmdpi_bone.h
	model/mmdpi_adjust.h
	model/mmdpi_define.h
	model/mmdpi_include.h
	model/mmdpi_model.h
	model/mmdpi_shader.h
	model/mmdpi_struct.h
	model/mmdpi_mesh.h
DESTINATION include/mmdpi/model)
install(FILES
	model/tools/bitmap.h
	model/tools/get_bin.h
	model/tools/raw_table.h
	model/tools/tga_image.h
	model/tools/utf8_sjis.h
	model/tools/dds.h
DESTINATION include/mmdpi/model/tools)
install(FILES
	model/bullet/mmdpi_bullet.h
DESTINATION include/mmdpi/model/bullet)
install(FILES
	pmd/pmd.h
	pmd/pmd_analyze.h
	pmd/pmd_draw.h
	pmd/pmd_ik.h
	pmd/pmd_load.h
	pmd/pmd_struct.h
DESTINATION include/mmdpi/pmd)
install(FILES
	pmx/pmx.h
	pmx/pmx_analyze.h
	pmx/pmx_draw.h
	pmx/pmx_ik.h
	pmx/pmx_load.h
	pmx/pmx_struct.h
DESTINATION include/mmdpi/pmx)
install(FILES
	vmd/vmd.h
DESTINATION include/mmdpi/vmd)

