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
	./GL 
	../libmmdpi/lpng
	../libmmdpi
)

set(mmdpi_lib_path
	/usr/local/lib
	../libmmdpi/lpng
	../libmmdpi
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


if( pi )
	#	Raspberry Pi
	set(mmdpi_inc_path
		${mmdpi_inc_path}
		/opt/vc/include/interface/vmcs_host/linux 
		/opt/vc/include 
		/opt/vc/include/interface/vcos/pthreads
	)

	set(mmdpi_lib_path
		${mmdpi_lib_path}
		/opt/vc/lib
	)
	
	#find_package(GLESv2)
	#find_package(EGL)
	#find_package(bcm_host)
	#find_package(rt)
	set(mmdpi_lib
		GLESv2
		EGL
		bcm_host
		rt
		#${GLESv2_LIBRARY}
		#${EGL_LIBRARY}
		#${bcm_host_LIBRARY}
		#${rt_LIBRARY}
	)

else()
	#	
	find_package(GL)
	find_package(OpenGL)
	find_package(GLEW)
	find_package(GLU)
	find_package(GLUT)

	set(mmdpi_lib
		${GL_LIBRARY}
		${OpenGL_LIBRARY}
		${GLEW_LIBRARY}
		${GLU_LIBRARY}
		${GLUT_LIBRARY}
	)

endif()

find_package(m)

#	静的ライブラリとしてビルド
link_directories(${mmdpi_lib_path})
include_directories( ${mmdpi_inc_path} )
add_executable(mmdpiv ${mmdpi_src})
target_link_libraries(mmdpiv mmdpi png z ${m_LIBRARY} ${mmdpi_lib})

