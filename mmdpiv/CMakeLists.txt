#	cmake 必要バージョン
cmake_minimum_required(VERSION 2.6)

#	プロジェクト名	と 言語(C=C, CXX=C++)
project(mmdpiv CXX)

#	リリースオプション
set(CMAKE_CXX_FLAG_RELEASE "-Wall -Os")

#	デバックオプション
set(CMAKE_CXX_FLAG_DEBUG "-g")

#	コンパイルオプション
set(CMAKE_BUILD_TYPE Release)

#	バージョン
set(SERIAL "1.00.00")

#	Source

if(NOT pi)
	set(mmdpi_src ${mmdpi_src} gl_main.cpp )
else()
	set(mmdpi_src ${mmdpi_src} gl_main_es.cpp )
endif()

#	Include Directory
set(mmdpi_inc_path
	/usr/local/include/
	/usr/include/
	../libmmdpi
)

set(mmdpi_lib_path
	/usr/local/lib
	/usr/lib
	../libmmdpi
)

#	Bullet
find_package( Bullet )
if( bullet AND Bullet_FOUND )
	add_definitions( -D_MMDPI_USING_PHYSICS_ENGINE_ )
	include_directories( Bullet_INCLUDE_DIRS )
	set( mmdpi_lib ${mmdpi_lib} ${Bullet_LIBRARIES} )
endif()
#	Outline
#	It needs a lot of VRAM.
if( outline )
	add_definitions( -D_MMDPI_OUTLINE_ )
endif()


find_package( GL )
find_package( OpenGL )
find_package( GLEW )
find_package( GLU )
find_package( GLUT )

#find_package( GLESv2 )
#find_package( EGL )
#find_package( bcm_host )	#	Raspberry pi graphics driver

find_package( PNG REQUIRED )
find_package( ZLIB REQUIRED )

if( NOT WIN32 )
	set(mmdpi_lib ${mmdpi_lib}
		rt m
	)
endif()

set(mmdpi_inc_path ${mmdpi_inc_path}	#	命名に両方の可能性
	${PNG_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
)
set( PNG_LIBRARIES ${PNG_LIBRARIES} ${PNG_LIBRARY} )
set( ZLIB_LIBRARIES ${ZLIB_LIBRARIES} ${ZLIB_LIBRARY})
set( m_LIBRARIES ${m_LIBRARIES} ${m_LIBRARY} )
set( rt_LIBRARIES ${rt_LIBRARIES} ${rt_LIBRARY})

#	Opengl Mode

#	OpenGL ES
if( NOT GL_FOUND )
	add_definitions( -D_MMDPI_OPENGL_ES_DEFINES_ )
	add_definitions( -D_MMDPI_PRIJECTION_MATRIX_SELF_ )
	if( pi )
		set( mmdpi_lib_path ${mmdpi_lib_path} /opt/vc/lib )
	endif()
	set( RASPGL_LIBRARIES GLESv2 EGL )
	if( pi )
		set( RASPGL_LIBRARIES ${RASPGL_LIBRARIES} bcm_host )
	endif()
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
	
	${GL_LIBRARIES}
	${OpenGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${GLU_LIBRARIES}
	${GLUT_LIBRARIES}
)

#	静的ライブラリとしてビルド
link_directories( ${mmdpi_lib_path} )
include_directories( ${mmdpi_inc_path} )
add_executable( mmdpiv ${mmdpi_src} )
target_link_libraries( mmdpiv 
	mmdpi 
	${m_LIBRARIES} ${rt_LIBRARIES} 
	${PNG_LIBRARIES} ${ZLIB_LIBRARIES} 
	${mmdpi_lib} ${RASPGL_LIBRARIES}
)
