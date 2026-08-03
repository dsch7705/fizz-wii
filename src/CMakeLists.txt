set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(WII_PORTLIBS $ENV{DEVKITPRO}/portlibs/wii)

add_executable(fizz-wii 
  main.cpp
  menu.cpp
)
target_include_directories(fizz-wii PRIVATE ${WII_PORTLIBS}/include)
target_link_directories(fizz-wii PRIVATE ${WII_PORTLIBS}/lib)
target_link_libraries(fizz-wii PRIVATE fizz grrlib fat ogc freetype bz2 z png brotlidec brotlicommon)

# elf2dol conversion
find_program(ELF2DOL elf2dol PATHS "$ENV{DEVKITPRO}/tools/bin")
if(ELF2DOL)
  add_custom_command(TARGET fizz-wii POST_BUILD
    COMMAND ${ELF2DOL} $<TARGET_FILE:fizz-wii> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot.dol
    COMMENT "Converting ELF to DOL format"
  )
else()
  message(WARNING "elf2dol tool not found. Skipping DOL conversion.")
endif()
