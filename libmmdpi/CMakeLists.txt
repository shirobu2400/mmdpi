# cmake version
cmake_minimum_required(VERSION 2.6)

# project name and language option(C=C, CXX=C++)
project(mmdpi C CXX)

# # Release options
# set(CMAKE_CXX_FLAG_RELEASE "-O2")

# # Debug options
# set(CMAKE_CXX_FLAG_DEBUG "-O0 -ggdb3 -fno-inline")

# compile option
# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

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

	model/bullet/mmdpi_bullet.cpp

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
	mmdpi.hpp

	model/mmdpi_bone.hpp
	model/mmdpi_adjust.hpp
	model/mmdpi_define.hpp
	model/mmdpi_include.hpp
	model/mmdpi_model.hpp
	model/mmdpi_shader.hpp
	model/mmdpi_struct.hpp
	model/mmdpi_mesh.hpp

	model/tools/bitmap.hpp
	model/tools/get_bin.hpp
	model/tools/raw_table.hpp
	model/tools/dds.hpp
	model/tools/tga_image.hpp
	model/tools/utf8_sjis.hpp

	model/bullet/mmdpi_bullet.hpp

	pmd/pmd.hpp
	pmd/pmd_analyze.hpp
	pmd/pmd_draw.hpp
	pmd/pmd_ik.hpp
	pmd/pmd_load.hpp
	pmd/pmd_struct.hpp

	pmx/pmx.hpp
	pmx/pmx_analyze.hpp
	pmx/pmx_draw.hpp
	pmx/pmx_ik.hpp
	pmx/pmx_load.hpp
	pmx/pmx_struct.hpp

	vmd/vmd.hpp
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
	model/bullet/mmdpi_bullet.hpp
)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
if(NOT pi)
	find_package(OpenGL REQUIRED)
	find_package(GLUT REQUIRED)
	find_package(GLEW REQUIRED)
	find_library(GLUT_glut_LIBRARY NAMES glut glut32 freeglut
		PATHS
		${OPENGL_LIBRARY_DIR}
		${GLUT_ROOT_PATH}/Release
	)
endif()

#	OpenGL
if(GLUT_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
else()
	add_definitions(-D_MMDPI_OPENGL_ES_DEFINES_)
	add_definitions(-D_MMDPI_PRIJECTION_MATRIX_SELF_)
endif()
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

# Bullet
find_package(Bullet REQUIRED)
set(libmmdpi_src ${libmmdpi_src} ${libmmdpi_bullet_src})

add_definitions(-D_MMDPI_USING_PHYSICS_ENGINE_)
include_directories(Bullet_INCLUDE_DIRS /usr/local/include/bullet)

# Outline
# It needs a lot of VRAM.
if(outline)
	add_definitions(-D_MMDPI_OUTLINE_)
endif()

set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} -m32)

# static library
add_library(mmdpi STATIC ${libmmdpi_src})
include_directories(${libmmdpi_inc})
include_directories(${libmmdpi_inc_raspi})

# dynamic library
add_library(mmdpis SHARED ${libmmdpi_src})
set_property(TARGET mmdpis PROPERTY POSITION_INDEPENDENT_CODE ON)

# make install
install(TARGETS mmdpi ARCHIVE DESTINATION lib/mmdpi)
install(TARGETS mmdpis LIBRARY DESTINATION lib/mmdpi)

install(FILES mmdpi.hpp DESTINATION include/mmdpi)
install(FILES
	model/mmdpi_bone.hpp
	model/mmdpi_adjust.hpp
	model/mmdpi_define.hpp
	model/mmdpi_include.hpp
	model/mmdpi_model.hpp
	model/mmdpi_shader.hpp
	model/mmdpi_struct.hpp
	model/mmdpi_mesh.hpp
DESTINATION include/mmdpi/model)
install(FILES
	model/tools/bitmap.hpp
	model/tools/get_bin.hpp
	model/tools/raw_table.hpp
	model/tools/tga_image.hpp
	model/tools/utf8_sjis.hpp
	model/tools/dds.hpp
DESTINATION include/mmdpi/model/tools)
install(FILES
	model/bullet/mmdpi_bullet.hpp
DESTINATION include/mmdpi/model/bullet)
install(FILES
	pmd/pmd.hpp
	pmd/pmd_analyze.hpp
	pmd/pmd_draw.hpp
	pmd/pmd_ik.hpp
	pmd/pmd_load.hpp
	pmd/pmd_struct.hpp
DESTINATION include/mmdpi/pmd)
install(FILES
	pmx/pmx.hpp
	pmx/pmx_analyze.hpp
	pmx/pmx_draw.hpp
	pmx/pmx_ik.hpp
	pmx/pmx_load.hpp
	pmx/pmx_struct.hpp
DESTINATION include/mmdpi/pmx)
install(FILES
	vmd/vmd.hpp
DESTINATION include/mmdpi/vmd)

