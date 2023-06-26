#include "Input.h"

void Input::MouseButtonDown(SDL_MouseButtonEvent &b) {
    if (b.button == SDL_BUTTON_LEFT) {
        mouseButtonDown_Left = true;
    }
    else if (b.button == SDL_BUTTON_RIGHT) {
        mouseButtonDown_Right = true;
    }
    else if (b.button == SDL_BUTTON_MIDDLE) {
        mouseButtonDown_Middle = true;
    }
    SDL_GetMouseState(&prevMousePosition.first, &prevMousePosition.second);
}

void Input::MouseButtonUp(SDL_MouseButtonEvent &b) {
    if (b.button == SDL_BUTTON_LEFT) {
        mouseButtonDown_Left = false;
    }
    else if (b.button == SDL_BUTTON_RIGHT) {
        mouseButtonDown_Right = false;
    }
    else if (b.button == SDL_BUTTON_MIDDLE) {
        mouseButtonDown_Middle = false;
    }
}

bool Input::GetMouseButtonDown(std::string button) {
    if (button == "Left" || button == "left") {
        return mouseButtonDown_Left;
    } else if (button == "Right" || button == "right") {
        return mouseButtonDown_Right;
    } else if (button == "Middle" || button == "middle") {
        return mouseButtonDown_Middle;
    } else return false;
}
