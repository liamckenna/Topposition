#pragma once
#include "GlobalVars.h"
#include "TerrainGeneration.h"
#include "MouseLogic.h"
#include "GameLogic.h"

bool loadMap();

void loadGamePieces();

void loadUI();

void GeneratePixels(int thread, int thread_count);

void renderPixels(int thread);

void ResetMap();

void loadGame();