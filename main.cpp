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

    int frames_drawn = 0;
    Uint32 fps_counter = 0;
    float fps = 0.0f;
    Uint32 prev_ticks = SDL_GetTicks64();
    while( !quit )
    {

        //Handle events on queue
        HandleEvents(playerInput);
        RenderScreen();


        Uint32 ticks_now = SDL_GetTicks64();
        Uint32 diff = ticks_now - prev_ticks;
        fps_counter = diff;
        prev_ticks = ticks_now;
        fps = 1000.0f/(float)fps_counter;

        //std::cout << (int)fps << std::endl;

    }
    //Free resources and close SDL
    close();

    return 0;
}
