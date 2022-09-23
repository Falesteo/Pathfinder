cmake_minimum_required(VERSION 3.16)
project(Pathfinder)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    set(BUILD_SHARED_LIBS OFF)
endif()

    # for Windows
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio REQUIRED)
include_directories(SFML/include)

# core library
add_library(core STATIC Grid.cpp Grid.h Cell.cpp Cell.h)

target_link_libraries(core PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# executable file
add_executable(Pathfinder main.cpp)
target_link_libraries(Pathfinder PUBLIC core)