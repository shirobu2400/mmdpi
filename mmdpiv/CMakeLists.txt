# cmake version
cmake_minimum_required(VERSION 2.6)

# project name option(C=C, CXX=C++)
project(mmdpiv CXX)

# # Release options
# set(CMAKE_CXX_FLAG_RELEASE "-O2")

# # Debug options
# set(CMAKE_CXX_FLAG_DEBUG "-O0 -ggdb3 -fno-inline")

# compile type
# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

# mmdpiv version
set(SERIAL "1.10.00")

# source
if(NOT pi)
	set(mmdpi_src ${mmdpi_src} glmain.cpp )
else()
	set(mmdpi_src ${mmdpi_src} glmain_pi.cpp )
endif()

# Include Directory
set(mmdpi_inc_path
	/usr/local/include
	/usr/include
	../libmmdpi
	/usr/include/bullet/
	/usr/include/bullet/BulletDynamics/
	/usr/include/bullet/BulletSoftBody/
	/usr/include/bullet/LinearMath/
	/usr/include/bullet/BulletCollision/
)

set(mmdpi_lib_path
	/usr/local/lib
	/usr/lib
	../libmmdpi
)

set(mmdpi_lib)

# Bullet
find_package(Bullet)
# if(bullet AND Bullet_FOUND)
if(Bullet_FOUND)
	add_definitions(-D_MMDPI_USING_PHYSICS_ENGINE_)
	include_directories(Bullet_INCLUDE_DIRS /usr/local/include/bullet)
	set(mmdpi_lib ${mmdpi_lib} BulletDynamics BulletCollision BulletSoftBody LinearMath)
endif()

# Outline
# It needs a lot of VRAM.
if(outline)
	add_definitions(-D_MMDPI_OUTLINE_)
endif()


find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_library(GLUT_glut_LIBRARY NAMES GLUT glut32 freeglut
    PATHS
    ${OPENGL_LIBRARY_DIR}
    ${GLUT_ROOT_PATH}/Release
)

#find_package(GLESv2)
#find_package(EGL)
#find_package(bcm_host)	# Raspberry pi graphics driver

if(NOT WIN32)
	set(mmdpi_lib ${mmdpi_lib} rt m)
endif()

set(mmdpi_inc_path ${mmdpi_inc_path}
	${PNG_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
)

#	Opengl Mode

#	OpenGL ES
set(mmdpi_lib_path)
if(GLUT_FOUND OR glut32_FOUND OR freeglut_FOUND)
	set(mmdpi_lib ${mmdpi_lib} GL GLU GLEW)
else()
	add_definitions(-D_MMDPI_OPENGL_ES_DEFINES_)
	add_definitions(-D_MMDPI_PRIJECTION_MATRIX_SELF_)
	if(pi)
		set(mmdpi_lib_path ${mmdpi_lib_path} /opt/vc/lib)
	endif()
	set(RASPGL_LIBRARIES GLESv2 EGL)
	if(pi)
		set(RASPGL_LIBRARIES ${RASPGL_LIBRARIES} bcm_host)
	endif()
endif()

if(x64)
	add_definitions(-D_MMDPI_AMD64BIT_)
else()
	set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} -m32)
endif()

#	Raspberry Pi
set(mmdpi_inc_path
	${mmdpi_inc_path}
	/opt/vc/include/interface/vmcs_host/linux
	/opt/vc/include
	/opt/vc/include/interface/vcos/pthreads
)

set(mmdpi_lib
	${mmdpi_lib}

	${GLUT_LIBRARIES}
	${OPENGL_LIBRARIES}
	${glut_LIBRARIES}
	${freeglut_LIBRARIES}
	${GLEW_LIBRARIES}

	${m_LIBRARIES}
	${rt_LIBRARIES}

	${PNG_LIBRARIES}
	${ZLIB_LIBRARIES}
	${JPEG_LIBRARIES}
)

#	link static library
link_directories(${mmdpi_lib_path})
include_directories(${mmdpi_inc_path})
add_executable(mmdpiv ${mmdpi_src})
target_link_libraries(mmdpiv
	mmdpi
	${mmdpi_lib} ${RASPGL_LIBRARIES}
)
install(TARGETS mmdpiv RUNTIME DESTINATION bin)
