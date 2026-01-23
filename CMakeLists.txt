cmake_minimum_required(VERSION 3.23)
project(Topposition LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Topposition WIN32
    main.cpp
    GameObject.cpp GameObject.h
    Input.cpp Input.h
    GameRules.cpp GameRules.h
    GlobalVars.cpp GlobalVars.h
    Player.cpp Player.h
    TerrainGeneration.cpp TerrainGeneration.h
    ProgramInitialization.cpp ProgramInitialization.h
    Rendering.cpp Rendering.h
    GameInitialization.cpp GameInitialization.h
    MouseLogic.cpp MouseLogic.h
    PlayerMovement.cpp PlayerMovement.h
    GameLogic.cpp GameLogic.h
    ClaimLogic.cpp ClaimLogic.h
    EventHandler.cpp EventHandler.h
    MultiPurposeFunctions.cpp MultiPurposeFunctions.h
    AnimationHandler.cpp AnimationHandler.h
    MenuInitialization.cpp MenuInitialization.h
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

target_link_libraries(Topposition PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)
