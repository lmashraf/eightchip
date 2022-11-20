# Find
file(
    GLOB_RECURSE EMULATOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
)

add_executable( ${EMULATOR_BINARY} ${EMULATOR_SOURCES} )

# Include external libraries
target_include_directories( ${EMULATOR_BINARY} PRIVATE ${sdl2_SOURCE_DIR}/include/ )

# Link external libraries
target_link_libraries( ${EMULATOR_BINARY}
            SDL2::SDL2main
            SDL2::SDL2-static
            ${OPENGL_LIBRARIES}
)

