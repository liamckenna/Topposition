#pragma once
#include "GlobalVars.h"
#include "ClaimLogic.h"
#include "Input.h"
#include "GameInitialization.h"
#include "PlayerMovement.h"
#include "MouseLogic.h"
#include "MultiPurposeFunctions.h"

void HandleEvents(Input *playerInput);

void EventQuit(Input *playerInput, SDL_MouseButtonEvent &event);

void EventWindowResized(Input *playerInput, SDL_WindowEvent &event);

void EventMouseWheel(Input *playerInput, SDL_MouseWheelEvent &event);

void EventMouseButtonDown(Input *playerInput, SDL_MouseButtonEvent &event);

void EventMouseButtonUp(Input *playerInput, SDL_MouseButtonEvent &event);

void MouseMovement(Input *playerInput);

void MouseButtonDownMainMenu(Input *playerInput, SDL_MouseButtonEvent &event);

void MouseButtonDownGame(Input *playerInput, SDL_MouseButtonEvent &event);

void MouseButtonUpMainMenu(Input *playerInput, SDL_MouseButtonEvent &event);

void MouseButtonUpGame(Input *playerInput, SDL_MouseButtonEvent &event);
