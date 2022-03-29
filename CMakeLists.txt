cmake_minimum_required(VERSION 3.10)

#project name
SET(PROJECTNAME tex_atlas)

project("${PROJECTNAME}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set (CMAKE_VERBOSE_MAKEFILE ON)

ENABLE_LANGUAGE(C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#include libpng (libpng-1.6.37 and zlib1211)
include_directories(${CMAKE_SOURCE_DIR}/libpng/inc)
include_directories(${CMAKE_SOURCE_DIR}/libpng/src)
include_directories(${CMAKE_SOURCE_DIR}/libpng/src/zlib)

set(LIB_PNG_SOURCE
				${LIB_PNG_SOURCE}
                             ${CMAKE_SOURCE_DIR}/libpng/src/png.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngerror.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngget.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngmem.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngpread.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngread.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngrio.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngrtran.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngrutil.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngset.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngtrans.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngwio.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngwrite.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngwtran.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/pngwutil.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/adler32.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/compress.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/crc32.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/deflate.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/gzclose.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/gzlib.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/gzread.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/gzwrite.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/infback.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/inffast.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/inflate.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/inftrees.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/trees.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/uncompr.c
                             ${CMAKE_SOURCE_DIR}/libpng/src/zlib/zutil.c
                             )

#executable
add_executable(${PROJECTNAME} 
                             ${CMAKE_SOURCE_DIR}/src/main.cpp
                             ${LIB_PNG_SOURCE}
                             )
