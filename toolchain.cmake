#
# Set the GCC installation path
#
SET(GCCTOOLPATH "d:/Software_Install/MinGW")

if (NOT GCCTOOLPATH)
  message(FATAL_ERROR "Please set the toolchain path in *.cmake" )
endif()

set(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR "AMD64")
set (X86 TRUE)

# Set the C and C++ compiler
SET(CMAKE_C_COMPILER   ${GCCTOOLPATH}/bin/gcc.exe)
SET(CMAKE_CXX_COMPILER ${GCCTOOLPATH}/bin/g++.exe)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  ${GCCTOOLPATH})

# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)