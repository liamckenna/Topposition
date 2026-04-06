#pragma once
#include "GlobalVars.h"
#include "MultiPurposeFunctions.h"
#include "Input.h"
#include "GameInitialization.h"
#include "PlayerMovement.h"

void zoom(SDL_MouseWheelEvent &event, Input *playerInput);

void scroll(Input *playerInput);

GameObject *selectObject(int x, int y, bool update = true);

UIElement *selectUI(int x, int y, bool update = true);

Piece *selectPiece(int x, int y, bool update = true);

Terrain *selectTerrain(int x, int y, bool update = true);

Item *selectItem(int x, int y, bool update = true);

void moveSelectedObject(GameObject *gameObject, Input *playerInput);