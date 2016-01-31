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

if(WIN32)
	set(mmdpi_inc_path ${mmdpi_inc_path} ../libmmdpi/define)
else(pi)
	set(mmdpi_inc_path ${mmdpi_inc_path} ../libmmdpi/define_es)
else()
	set(mmdpi_inc_path ${mmdpi_inc_path} ../libmmdpi/define_unix)
endif()

if(pi)
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
add_executable(./mmdpi ${mmdpi_src})
target_link_libraries(./mmdpi mmdpi png z ${m_LIBRARY} ${mmdpi_lib})

