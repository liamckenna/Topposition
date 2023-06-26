#pragma once
#include <string>
#include <utility>
#include <SDL.h>
class Input {
    bool mouseButtonDown_Left = false;
    bool mouseButtonDown_Right = false;
    bool mouseButtonDown_Middle = false;

public:
    std::pair<int, int> currentMousePosition;
    std::pair<int, int> prevMousePosition;

    void MouseButtonDown(SDL_MouseButtonEvent& b);
    void MouseButtonUp(SDL_MouseButtonEvent& b);
    bool GetMouseButtonDown(std::string button);
};

