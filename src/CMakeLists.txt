set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(WII_PORTLIBS $ENV{DEVKITPRO}/portlibs/wii)

add_executable(fizz-wii 
  main.cpp
  menu.cpp
)
target_include_directories(fizz-wii PRIVATE ${WII_PORTLIBS}/include)
target_link_directories(fizz-wii PRIVATE ${WII_PORTLIBS}/lib)
target_link_libraries(fizz-wii PRIVATE fizz grrlib fat ogc freetype bz2 z png brotlidec brotlicommon)

# Homebrew app folder creation
find_program(ELF2DOL elf2dol PATHS "$ENV{DEVKITPRO}/tools/bin")
if(ELF2DOL)
  set(HOMEBREW_DIR ${CMAKE_BINARY_DIR}/fizz-wii)
  file(MAKE_DIRECTORY "${HOMEBREW_DIR}")

  add_custom_command(
    TARGET fizz-wii
    POST_BUILD
    COMMAND ${ELF2DOL} $<TARGET_FILE:fizz-wii> ${HOMEBREW_DIR}/boot.dol
    COMMENT "Creating homebrew directory."
  )

  add_custom_command(
    TARGET fizz-wii
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/assets/meta.xml"
      "${HOMEBREW_DIR}/meta.xml"
  )

  add_custom_command(
    TARGET fizz-wii
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/assets/icon.png"
      "${HOMEBREW_DIR}/icon.png"
  )

  add_custom_command(
    TARGET fizz-wii
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/assets/font/OFL.txt"
      "${HOMEBREW_DIR}/OFL.txt"
  )
else()
  message(WARNING "elf2dol tool not found. Skipping homebrew directory creation.")
endif()
