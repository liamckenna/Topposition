#pragma once
#include "GlobalVars.h"
#include "TerrainGeneration.h"
#include "MouseLogic.h"
#include "GameLogic.h"

bool loadMap();

void loadGamePieces();

void loadText();

void loadUI();

void GeneratePixels();

void GenerateOcean();

void renderPixels();

void ResetMap();

void loadGame();