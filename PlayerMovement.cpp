#include "PlayerMovement.h"

void Move(Piece* piece, Terrain* startingPoint, Terrain* targetTerrain, int& movesLeft) {

    int moveCount = 0;
    int heightDifference;
    std::vector<Terrain*> currentPath;

    bool moveSuccess = MovementAttempt(heightDifference, moveCount, startingPoint, targetTerrain, currentPath, false);

    if (!moveSuccess) {
        piece->SetCenter(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second + piece->GetDimensions().second/2 * piece->GetSize());
    } else {
        movesLeft = movesLeft - moveCount;
        piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetCenter().second);
        piece->SetOccupyingTerrain(targetTerrain);
        if (startingPoint != NULL) {
            for (int i = 0; i < startingPoint->occupants.size(); i++) {
                if (startingPoint->occupants[i] == piece) {
                    startingPoint->occupants.erase(startingPoint->occupants.begin() + i);
                }
            }
        }
        if (targetTerrain != NULL) targetTerrain->occupants.push_back(piece);
        if (targetTerrain != NULL && targetTerrain->type == GameObject::PEAK) {
            RefreshClaimNotifs(dynamic_cast<Peak *>(targetTerrain));
        } else {
            RefreshClaimNotifs();
        }
        if (targetTerrain == NULL)  {
            piece->SetCurrentAnimation(piece->animations["floatIdle"]);
            piece->SetSurface(surfaces[currentTurn->GetName() + " piece float"]);
        }
        else {
            piece->SetCurrentAnimation(piece->animations["saluteIdle"]);
            piece->SetSurface(surfaces[currentTurn->GetName() + " piece salute"]);
        }
    }




    UpdateMovesLeft();

}

bool MovementAttempt(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain, std::vector<Terrain*>& currentPath, bool fromAdjacent) {

    int targetTerrainLayer;
    int currentTerrainLayer;
    if (targetTerrain == NULL) targetTerrainLayer = 0;
    else targetTerrainLayer = targetTerrain->GetLayer();
    if (currentTerrain == NULL) currentTerrainLayer = 0;
    else currentTerrainLayer =  currentTerrain->GetLayer();

    heightDifference = targetTerrainLayer - currentTerrainLayer;

    if (heightDifference == 0) {
        if (currentTerrain == targetTerrain && attemptedMoves <= movesLeft) return true;
    }

    if (currentTerrainLayer == 0) {
        currentPath.push_back(GetTargetTerrainBase(targetTerrain));
        return MovementAttempt(heightDifference, ++attemptedMoves, GetTargetTerrainBase(targetTerrain), targetTerrain, currentPath, false);
    }

    if (heightDifference > 0) {
        if (DirectMovementUp(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;
    }

    if (heightDifference < 0) {
        if (DirectMovementDown(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;
    }
    if (!fromAdjacent) {
        if (AdjacentMovement(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;
    }

    if (currentTerrain->GetLowerTerrain() != NULL && !fromAdjacent) {

        currentPath.push_back(currentTerrain->GetLowerTerrain());

        attemptedMoves++;
        if (MovementAttempt(heightDifference, attemptedMoves, currentTerrain->GetLowerTerrain(), targetTerrain, currentPath, false) && attemptedMoves <= movesLeft) return true;


    } else if (!fromAdjacent) {

        attemptedMoves = attemptedMoves + 2;
        if (attemptedMoves > movesLeft) return false;
        currentPath.push_back(nullptr);
        currentPath.push_back(GetTargetTerrainBase(targetTerrain));
        Terrain* terrainBase = GetTargetTerrainBase(targetTerrain);
        if (MovementAttempt(heightDifference, attemptedMoves, terrainBase, targetTerrain, currentPath, false) && attemptedMoves <= movesLeft) return true;
    }

    return false;

}

bool DirectMovementUp(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath) {


    int i;
    for (i = 0; i < heightDifference; i++) {
        if (currentTerrain->GetUpperTerrain() == NULL) break;
        currentTerrain = currentTerrain->GetUpperTerrain();
        currentPath.push_back(currentTerrain);
        attemptedMoves++;
        if (currentTerrain == targetTerrain) break;
    }


    if (currentTerrain == targetTerrain) return true;
    else {
        for (int j = 0; j < i; j++) {
            currentPath.pop_back();
            attemptedMoves--;

        }
    }
    return false;
}

bool DirectMovementDown(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath) {
    int i;
    for (i = 0; i > heightDifference; i--) {
        currentTerrain = currentTerrain->GetLowerTerrain();
        currentPath.push_back(currentTerrain);
        attemptedMoves++;
    }
    if (currentTerrain == targetTerrain) return true;
    else {
        for (int j = 0; j > i; j--) {
            currentPath.pop_back();
            attemptedMoves--;

        }
    }
    return false;
}

Terrain* GetTargetTerrainBase(Terrain* targetTerrain) {
    if (targetTerrain == NULL) return targetTerrain;
    Terrain* targetTerrainBase = targetTerrain;
    while (targetTerrainBase->GetLowerTerrain() != nullptr) {
        targetTerrainBase = targetTerrainBase->GetLowerTerrain();
    }
    return targetTerrainBase;
}

bool AdjacentMovement(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath) {
    for (int i = 0; i < currentTerrain->connectedTerrain.size(); i++) {

        bool alreadyCovered = false;

        for (int j = 0; j < currentPath.size(); j++) {
            if (currentPath[j] == currentTerrain->connectedTerrain[i]) {
                alreadyCovered = true;
            }
        }
        if (alreadyCovered) continue;

        currentPath.push_back(currentTerrain->connectedTerrain[i]);

        if (MovementAttempt(heightDifference, attemptedMoves, currentTerrain->connectedTerrain[i], targetTerrain, currentPath, true) && attemptedMoves <= movesLeft) return true;
        else currentPath.pop_back();
    }

    return false;
}
