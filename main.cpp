#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <utility>
#include <map>
#include <math.h>
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
#include <random>

float last_fps_time = 0.f;
int frame_count = 0;

void CalculateFrameRate();

int main(int argc, char *args[])
{
    // Start up SDL and create window

    if (!init())
    {
        printf("Failed to initialize!\n");
        return 1;
    }

    LoadMenu();

    Input *playerInput = new Input();
    // While application is running

    float fps = 0.0f;
    Uint64 lastUpdate = SDL_GetTicks();
    int frame = 0;
    Uint64 lastFrame = 0;
    Uint64 start = SDL_GetPerformanceCounter();
    while (!quit)
    {
        CalculateFrameRate();
        // Handle events on queue
        HandleEvents(playerInput);
        frame++;
        AnimationHandler(fps, lastFrame, lastUpdate);
        RenderScreen();
    }
    // Free resources and close SDL
    close();

    return 0;
}

void CalculateFrameRate()
{

    currentTime = SDL_GetTicks();
    deltaTime = currentTime - prevTime;

    static Uint32 startTime = SDL_GetTicks(); // Time when we started counting
    static int frameCount = 0;                // Frame counter
    static int fps = 0;                       // The FPS value

    frameCount++;

    // Get the current time in milliseconds
    Uint32 currentTime = SDL_GetTicks();

    // Calculate the elapsed time (in milliseconds) since we started counting
    Uint32 elapsedTime = currentTime - startTime;

    // Check if a second has passed
    if (elapsedTime >= 1000)
    {
        // Calculate frames per second (FPS)
        fps = frameCount;

        // Print the FPS count
        std::cout << "FPS: " << fps << std::endl;

        // Reset for the next second
        frameCount = 0;
        startTime = currentTime;
        // std::cout << "deltaTime: " << deltaTime << std::endl;
        std::cout << "ocean size: " << ocean.size() << std::endl;
    }

    prevTime = currentTime;
}