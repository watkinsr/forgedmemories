cmake_minimum_required(VERSION 3.7)
project(forgedmemories)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Wextra")

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
if (NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found. Please install SDL2.")
endif()
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
if (NOT SDL2_MIXER_FOUND)
    message(FATAL_ERROR "SDL2 Mixer not found. Please install SDL2 Mixer.")
endif()
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
if (NOT SDL2_TTF_FOUND)
    message(FATAL_ERROR "SDL2 TTF not found. Please install SDL2 TTF.")
endif()
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
if (NOT SDL2_IMAGE_FOUND)
    message(FATAL_ERROR "SDL2 Image not found. Please install SDL2 Image.")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_MIXER_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Collect all .cpp files in the src directory
file(GLOB SOURCES "src/*.cpp")

add_executable(forgedmemories src/Game.cpp src/Common.cpp)
add_executable(mapeditor src/MapEditor.cpp src/Common.cpp)

target_link_libraries(forgedmemories ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(mapeditor ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
