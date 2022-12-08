cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE "Release")

project(CZIResize)

add_compile_definitions(_LIBCZISTATICLIB)
add_compile_definitions(LIBCZI_BUILD_UNITTESTS=OFF)
add_compile_definitions(LIBCZI_BUILD_DYNLIB=OFF)
add_compile_definitions(LIBCZI_BUILD_CZICMD=OFF)
add_subdirectory(libczi)
include_directories(libczi/Src)

set_target_properties(libCZI PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_executable(cziresize main.cpp)
target_link_libraries(cziresize PRIVATE libCZIStatic JxrDecodeStatic)