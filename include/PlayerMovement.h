#pragma once
#include "GlobalVars.h"
#include "ClaimLogic.h"

bool Move(Piece* piece, Terrain* startingPoint, Terrain* targetTerrain, int& movesLeft);

bool MovementAttempt(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain, std::vector<Terrain*>& currentPath, bool fromAdjacent);

bool CheckMovementPossibility(Piece* piece, Terrain* targetTerrain);

bool DirectMovementUp(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath);

bool DirectMovementDown(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath);

Terrain* GetTargetTerrainBase(Terrain* targetTerrain);

bool AdjacentMovement(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath);
