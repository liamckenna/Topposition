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



    if( !loadMap(gameObjects) )
    {
        printf( "Failed to load media!\n" );
        return 1;
    }
    rules->SetPieces(6);
    rules->SetMaxHeight(6);
    loadGamePieces(gameObjects);
    //Main loop flag


    //Event handler

    Input* playerInput = new Input();
    //While application is running
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );

    while( !quit )
    {

        //Handle events on queue
        HandleEvents(gameObjects, playerInput);

        RenderScreen(gameObjects);

    }
    //Free resources and close SDL
    close();

    return 0;
}
