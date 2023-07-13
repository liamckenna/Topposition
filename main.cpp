#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <utility>
#include <map>
#include <math.h>
#include "GameObject.h"
#include "Input.h"
#include "GameRules.h"
#include "Functions.h"
#include <random>


//hi

int main( int argc, char* args[] )
{
    //Start up SDL and create window

    if( !init() )
    {
        printf( "Failed to initialize!\n" );
        return 1;
    }


    ResetMap();



    //Event handler

    Input* playerInput = new Input();
    //While application is running


    float fps = 0.0f;
    Uint64 lastUpdate = SDL_GetTicks64();
    int frame = 0;
    Uint64 lastFrame = 0;
    Uint64 start = SDL_GetPerformanceCounter();
    while( !quit )
    {

        Uint64 end = SDL_GetPerformanceCounter();
        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        float elapsedFrames = (end - start) / (float)SDL_GetPerformanceFrequency();
        fps = 1.0f/ elapsedFrames;
        //std::cout << fps << std::endl;
        start = SDL_GetPerformanceCounter();
        //Handle events on queue
        HandleEvents(playerInput);
        frame++;
        AnimationHandler(fps, lastFrame, lastUpdate);
        RenderScreen();




    }
    //Free resources and close SDL
    close();

    return 0;
}
