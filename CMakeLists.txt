cmake_minimum_required(VERSION 3.14)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 14)

find_library(SDL2_LIBRARY SDL2 "/usr/local/Cellar/sdl2/2.0.9_1/lib/libSDL2-2.0.0.dylib")
find_library(${PROJECT_NAME} ${SDL2_LIBRARY})
add_executable(GameOfLife main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
