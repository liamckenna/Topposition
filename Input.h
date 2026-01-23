#pragma once
#include <string>
#include <utility>
#include <SDL3/SDL.h>
class Input
{
    bool mouseButtonDown_Left = false;
    bool mouseButtonDown_Right = false;
    bool mouseButtonDown_Middle = false;

public:
    std::pair<float, float> currentMousePosition;
    std::pair<float, float> prevMousePosition;

    void MouseButtonDown(SDL_MouseButtonEvent &b);
    void MouseButtonUp(SDL_MouseButtonEvent &b);
    bool GetMouseButtonDown(std::string button);
};
