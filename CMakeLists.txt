# add_subdirectory でビルド対象追加
cmake_minimum_required(VERSION 2.6)

# サブディレクトリを指定．指定したディレクトリ内のCMakeLists.txt が実行される
add_subdirectory(libmmdpi/lpng)
add_subdirectory(libmmdpi)
add_subdirectory(mmdpiv)
