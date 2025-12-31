set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(WII_PORTLIBS /opt/devkitpro/portlibs/wii)

add_executable(fizz-wii main.cpp
  ui/menu.cpp)
target_include_directories(fizz-wii PRIVATE ${WII_PORTLIBS}/include)
target_link_directories(fizz-wii PRIVATE ${WII_PORTLIBS}/lib)
target_link_libraries(fizz-wii PRIVATE fizz grrlib pngu fat ogc freetype)