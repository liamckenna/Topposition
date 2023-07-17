#pragma once
#include "GlobalVars.h"
#include "MultiPurposeFunctions.h"
#include "Input.h"

void zoom(SDL_Event e, Input* playerInput);

void scroll(Input* playerInput);

GameObject* selectObject(int x, int y);

UIElement* selectUI(int x, int y);

Piece* selectPiece(int x, int y);

Terrain* selectTerrain(int x, int y);

Item* selectItem(int x, int y);

void moveSelectedObject(GameObject* gameObject, Input* playerInput);

void RecenterScreen(Input* playerInput);
