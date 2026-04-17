#pragma once
#include "GameObject.h"
#include "GlobalVars.h"
#include "MultiPurposeFunctions.h"

void GeneratePeak();

void GenerateTerrain(Peak* peak, int shape, int height);

std::vector<Terrain*> MergeTerrain(Terrain* peak);

bool TerrainIsSurrounded(Terrain* peak, Terrain* other);

void GroomTerrain();

void ConnectTerrain();

void ConnectConnectedTerrain(std::vector<Terrain*>& connectedTerrain, Terrain* currentTerrain);

void NeighborTerrain();

void SetTerrainBiome();