cmake_minimum_required(VERSION 3.23)
project(Topposition)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

set(SDL2_GPU_INCLUDE_PATH "sdl-gpu/include")
set(SDL2_GPU_LIBRARY_PATH "C:/Program Files/JetBrains/CLion 2023.1.3/bin/mingw/lib/SDL2_gpu.lib")

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_GPU_INCLUDE_PATH})

add_executable(Topposition main.cpp GameObject.cpp GameObject.h Input.cpp Input.h GameRules.cpp GameRules.h GlobalVars.cpp GlobalVars.h Player.cpp Player.h TerrainGeneration.cpp TerrainGeneration.h ProgramInitialization.cpp ProgramInitialization.h Rendering.cpp Rendering.h GameInitialization.cpp GameInitialization.h MouseLogic.cpp MouseLogic.h PlayerMovement.cpp PlayerMovement.h GameLogic.cpp GameLogic.h ClaimLogic.cpp ClaimLogic.h EventHandler.cpp EventHandler.h MultiPurposeFunctions.cpp MultiPurposeFunctions.h AnimationHandler.cpp AnimationHandler.h)



target_link_libraries(Topposition ${SDL2_LIBRARY} ${SDL2_GPU_LIBRARY_PATH} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_gpu -lSDL2_ttf -mwindows)