cmake_minimum_required( VERSION 3.10 )

# Project
project ( eight_chip
            LANGUAGES CXX
            VERSION 1.0
        )

set( CMAKE_CXX_STANDARD 14 )

set( EMULATOR_BINARY ${CMAKE_PROJECT_NAME}_run )

# External dependencies
include( FetchContent )

## SDL
message( STATUS "Fetching SDL ..." )
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.24.2
)

FetchContent_GetProperties( sdl2 )
if( NOT sdl2_POPULATED )
    FetchContent_Populate( sdl2 )
    add_subdirectory( ${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR} )
endif( )

## OpenGL
message( STATUS "Fetching OpenGL ..." )
set( OpenGL_GL_PREFERENCE GLVND )
find_package( OpenGL REQUIRED )

# Settings
#configure_file(
  #${CMAKE_CURRENT_SOURCE_DIR}/settings.ini
  #${CMAKE_CURRENT_BINARY_DIR}/src/
  #COPYONLY
#)

# Includes
include_directories( includes/ ${OPENGL_INCLUDE_DIRS} )

# Sources
add_subdirectory( src )