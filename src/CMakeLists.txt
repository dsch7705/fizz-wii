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
    COMMENT "Copying boot.dol to homebrew directory"
  )

  add_custom_command(
    TARGET fizz-wii
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/meta.xml"
      "${HOMEBREW_DIR}/meta.xml"
    COMMENT "Copying meta.xml to homebrew directory"
  )

  add_custom_command(
    TARGET fizz-wii
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/icon.png"
      "${HOMEBREW_DIR}/icon.png"
    COMMENT "Copying icon.png to homebrew directory"
  )
else()
  message(WARNING "elf2dol tool not found. Skipping DOL conversion.")
endif()
