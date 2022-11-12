file(
    GLOB_RECURSE EMULATOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
)

add_executable( ${EMULATOR_BINARY} ${EMULATOR_SOURCES} )

target_include_directories( ${EMULATOR_BINARY} PRIVATE ${sdl_SOURCE_DIR}/include/ )
target_link_libraries( ${EMULATOR_BINARY} sdl2 )