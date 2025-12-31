cmake_minimum_required(VERSION 3.26)
project(fizz-wii)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(BUILD_FIZZ_EXAMPLES OFF)
FetchContent_Declare(
  fizz
  GIT_REPOSITORY https://github.com/dsch7705/fizz.git
)
FetchContent_MakeAvailable(fizz)

add_subdirectory(src)