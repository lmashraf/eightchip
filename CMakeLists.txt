cmake_minimum_required( VERSION 3.10 )

# Project
project ( eight_chip
            LANGUAGES CXX
            VERSION 1.0
        )

set( CMAKE_CXX_STANDARD 14 )

set( EMULATOR_BINARY ${CMAKE_PROJECT_NAME}_run )

# External dependencies
include(FetchContent)

## SDL
message(STATUS "Fetching SDL ...")
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.0.22
)

FetchContent_GetProperties(sdl2)
if(NOT sdl2_POPULATED)
    FetchContent_Populate(sdl2)
    add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
endif()

## OpenGL
message(STATUS "Fetching OpenGL ...")
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Settings
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/settings.ini
  ${CMAKE_CURRENT_BINARY_DIR}/src/
  COPYONLY
)

# Includes
include_directories( includes/ ${OPENGL_INCLUDE_DIRS} )

# Sources
add_subdirectory( src )