cmake_minimum_required(VERSION 3.26)
project(fizz-wii)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_FIZZ_EXAMPLES OFF)
add_subdirectory(external/fizz)

add_subdirectory(src)