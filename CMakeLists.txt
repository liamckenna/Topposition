cmake_minimum_required(VERSION 3.23)
project(Topposition)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
add_executable(Topposition main.cpp GameObject.cpp GameObject.h Input.cpp Input.h GameRules.cpp GameRules.h Functions.cpp Functions.h)

target_link_libraries(Topposition ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows)