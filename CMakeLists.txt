cmake_minimum_required(VERSION 3.23)

project(Topposition)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

set(SDL2_GPU_INCLUDE_PATH "sdl-gpu/include")

add_library(sdl_gpu SHARED ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_GLES_1.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_GLES_2.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_GLES_3.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_OpenGL_1.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_OpenGL_1_BASE.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_OpenGL_2.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_OpenGL_3.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_OpenGL_4.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_RendererImpl.h ${SDL2_GPU_INCLUDE_PATH}/SDL_gpu_version.h)

set_target_properties(sdl_gpu PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_GPU_INCLUDE_PATH})


add_executable(Topposition main.cpp GameObject.cpp GameObject.h Input.cpp Input.h GameRules.cpp GameRules.h GlobalVars.cpp GlobalVars.h Player.cpp Player.h TerrainGeneration.cpp TerrainGeneration.h ProgramInitialization.cpp ProgramInitialization.h Rendering.cpp Rendering.h GameInitialization.cpp GameInitialization.h MouseLogic.cpp MouseLogic.h PlayerMovement.cpp PlayerMovement.h GameLogic.cpp GameLogic.h ClaimLogic.cpp ClaimLogic.h EventHandler.cpp EventHandler.h MultiPurposeFunctions.cpp MultiPurposeFunctions.h AnimationHandler.cpp AnimationHandler.h)



target_link_libraries(Topposition ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image sdl_gpu -lSDL2_ttf -mwindows)