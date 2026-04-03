#pragma once
#include "GlobalVars.h"
#include "MultiPurposeFunctions.h"
#include "Input.h"
#include "GameInitialization.h"
#include "PlayerMovement.h"

void zoom(SDL_MouseWheelEvent &event, Input *playerInput);

void scroll(Input *playerInput);

GameObject *selectObject(int x, int y);

UIElement *selectUI(int x, int y);

Piece *selectPiece(int x, int y);

Terrain *selectTerrain(int x, int y);

Item *selectItem(int x, int y);

void moveSelectedObject(GameObject *gameObject, Input *playerInput);