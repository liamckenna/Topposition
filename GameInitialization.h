#pragma once
#include "GlobalVars.h"
#include "TerrainGeneration.h"
#include "MouseLogic.h"
#include "GameLogic.h"

bool loadMap();

void loadGamePieces();

void loadUI();

void GeneratePixels();

void renderPixels();

void ResetMap();