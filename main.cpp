#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <utility>
#include <map>
#include <math.h>
#include <random>
#include "GameObject.h"
#include "Input.h"
#include "GameRules.h"
#include "GlobalVars.h"
#include "Rendering.h"
#include "ProgramInitialization.h"
#include "GameInitialization.h"
#include "MenuInitialization.h"
#include "EventHandler.h"
#include "AnimationHandler.h"

float last_fps_time = 0.f;
int frame_count = 0;

void CalculateFrameRate();

int main(int argc, char *args[])
{
    if (!init())
    {
        printf("Failed to initialize!\n");
        return 1;
    }

    LoadMenu();

    Input *playerInput = new Input();

    float fps = 0.0f;
    Uint64 lastUpdate = SDL_GetTicks();
    int frame = 0;
    Uint64 lastFrame = 0;
    Uint64 start = SDL_GetPerformanceCounter();
    while (!quit)
    {
        HandleEvents(playerInput);
        frame++;
        AnimationHandler(fps, lastFrame, lastUpdate);
        RenderScreen();
        CalculateFrameRate();
    }
    close();

    return 0;
}

void CalculateFrameRate()
{

    currentTime = SDL_GetTicks();
    deltaTime = currentTime - prevTime;

    //if (deltaTime < 5)
    //    SDL_Delay(5 - deltaTime);

    currentTime = SDL_GetTicks();
    deltaTime = currentTime - prevTime;

    frameCounter++;
    frameCountTime += deltaTime;
    if (frameCountTime >= 1000)
    {
        std::cout << "FPS: " << frameCounter << std::endl;
        frameCounter = 0;
        frameCountTime = 0;
    }

    prevTime = currentTime;
}