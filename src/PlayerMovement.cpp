#include "PlayerMovement.h"

bool Move(Piece* piece, Terrain* startingPoint, Terrain* targetTerrain, int& movesLeft) {

    int moveCount = 0;
    int heightDifference;
    int bestCost = -1;
    std::vector<Terrain*> currentPath;

    bool moveSuccess = MovementAttempt(heightDifference, moveCount, startingPoint, targetTerrain, currentPath, false, bestCost);

    if (!moveSuccess) {
        if (bestCost != -1) moveCount = bestCost;
        std::pair<float, float> originalPosition = piece->GetPosition();
        piece->SetCenter(piece->GetDesignatedLocation().first, piece->GetCenter().second);
        piece->SetBottomRight(piece->GetBottomRight().first, piece->GetDesignatedLocation().second);
    } else {
        movesLeft = movesLeft - moveCount;
        piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetBottomRight().second);
        piece->SetOccupyingTerrain(targetTerrain);
        if (startingPoint != NULL) {
            for (int i = 0; i < startingPoint->occupants.size(); i++) {
                if (startingPoint->occupants[i] == piece) {
                    startingPoint->occupants.erase(startingPoint->occupants.begin() + i);
                }
            }
        }
        if (targetTerrain != NULL) targetTerrain->occupants.push_back(piece);
        RefreshClaimAndDefendNotifs();
        if (targetTerrain == NULL)  {
            if (piece->GetPlayer() == currentTurn)
            {
                piece->SetCurrentAnimation(piece->animations["floatSaluteIdle"]);
            }
            else 
            {
                piece->SetCurrentAnimation(piece->animations["floatIdle"]);
            }
            piece->SetSurface(surfaces["float"]);
        }
        else {
            if (piece->GetPlayer() == currentTurn)
            {
                piece->SetCurrentAnimation(piece->animations["saluteIdle"]);
            }
            else 
            {
                piece->SetCurrentAnimation(piece->animations["standIdle"]);
            }
            piece->SetSurface(surfaces["salute"]);
        }
    }




    UpdateMovesLeft();

    return moveSuccess;

}

bool MovementAttempt(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain, std::vector<Terrain*>& currentPath, bool fromAdjacent, int& bestCost) {

    int targetTerrainLayer;
    int currentTerrainLayer;
    if (targetTerrain == NULL) targetTerrainLayer = 0;
    else targetTerrainLayer = targetTerrain->GetLayer();
    if (currentTerrain == NULL) currentTerrainLayer = 0;
    else currentTerrainLayer =  currentTerrain->GetLayer();

    heightDifference = targetTerrainLayer - currentTerrainLayer;

    if (heightDifference == 0) {
        if (currentTerrain == targetTerrain) {
            if (attemptedMoves <= movesLeft) return true;
            if (bestCost == -1 || attemptedMoves < bestCost) bestCost = attemptedMoves;
        }
    }

    if (currentTerrainLayer == 0) {
        Terrain* targetBase = GetTargetTerrainBase(targetTerrain);
        if (targetBase != currentTerrain) {
            currentPath.push_back(targetBase);
            return MovementAttempt(heightDifference, ++attemptedMoves, targetBase, targetTerrain, currentPath, false, bestCost);
        }
    }

    if (heightDifference > 0) {
        int savedMoves = attemptedMoves;
        int savedPathSize = (int)currentPath.size();
        if (DirectMovementUp(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath)) {
            if (attemptedMoves <= movesLeft) return true;
            if (bestCost == -1 || attemptedMoves < bestCost) bestCost = attemptedMoves;
            currentPath.resize(savedPathSize);
            attemptedMoves = savedMoves;
        }
    }

    if (heightDifference < 0) {
        int savedMoves = attemptedMoves;
        int savedPathSize = (int)currentPath.size();
        if (DirectMovementDown(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath)) {
            if (attemptedMoves <= movesLeft) return true;
            if (bestCost == -1 || attemptedMoves < bestCost) bestCost = attemptedMoves;
            currentPath.resize(savedPathSize);
            attemptedMoves = savedMoves;
        }
    }

    if (!fromAdjacent) {
        if (AdjacentMovement(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath, bestCost) && attemptedMoves <= movesLeft) return true;
    }

    if (currentTerrain->GetLowerTerrain() != NULL && !fromAdjacent)
    {
        int savedMoves = attemptedMoves;
        int savedPathSize = (int)currentPath.size();
        currentPath.push_back(currentTerrain->GetLowerTerrain());
        attemptedMoves++;
        if (MovementAttempt(heightDifference, attemptedMoves, currentTerrain->GetLowerTerrain(), targetTerrain, currentPath, false, bestCost) && attemptedMoves <= movesLeft) return true;
        currentPath.resize(savedPathSize);
        attemptedMoves = savedMoves;
    }
    else if (!fromAdjacent)
    {
        int savedMoves = attemptedMoves;
        attemptedMoves = attemptedMoves + 2;
        if (attemptedMoves > movesLeft) { attemptedMoves = savedMoves; return false; }
        currentPath.push_back(nullptr);
        currentPath.push_back(GetTargetTerrainBase(targetTerrain));
        Terrain* terrainBase = GetTargetTerrainBase(targetTerrain);
        if (MovementAttempt(heightDifference, attemptedMoves, terrainBase, targetTerrain, currentPath, true, bestCost) && attemptedMoves <= movesLeft) return true;
        currentPath.pop_back();
        currentPath.pop_back();
        attemptedMoves = savedMoves;
    }

    return false;

}

bool CheckMovementPossibility(Piece* piece, Terrain* targetTerrain)
{
    SDL_Color White = {255, 255, 255};
    SDL_Color Red = {255, 0, 0};
    int moveCount = 0;
    int heightDifference;
    int bestCost = -1;
    std::vector<Terrain*> currentPath;
    Terrain* currentTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
    string moveCostString = "-";
    bool moveIsPossible = MovementAttempt(heightDifference, moveCount, currentTerrain, targetTerrain, currentPath, false, bestCost);
    if (bestCost != -1)
        moveCount = bestCost;

    if (bestCost == -1 && !moveIsPossible && currentTerrain != nullptr && targetTerrain != nullptr) {
        Terrain* currentBase = GetTargetTerrainBase(currentTerrain);
        Terrain* targetBase = GetTargetTerrainBase(targetTerrain);
        if (currentBase != nullptr && targetBase != nullptr) {
            moveCount = (currentTerrain->GetLayer() - currentBase->GetLayer()) + (targetTerrain->GetLayer() - targetBase->GetLayer());
            if (currentBase != targetBase) moveCount += 2;
        }
    }

    moveCostString = moveCostString + to_string(moveCount);
    if (moveIsPossible)
        moveCostText->SetColor(White, renderer);
    else
        moveCostText->SetColor(Red, renderer);

    moveCostText->SetTextContent(moveCostString.c_str(), renderer);

    return moveIsPossible;
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

bool AdjacentMovement(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain, std::vector<Terrain*>& currentPath, int& bestCost) {
    for (int i = 0; i < currentTerrain->connectedTerrain.size(); i++) {

        bool alreadyCovered = false;

        for (int j = 0; j < currentPath.size(); j++) {
            if (currentPath[j] == currentTerrain->connectedTerrain[i]) {
                alreadyCovered = true;
            }
        }
        if (alreadyCovered) continue;

        int savedMoves = attemptedMoves;
        int savedPathSize = (int)currentPath.size();
        currentPath.push_back(currentTerrain->connectedTerrain[i]);

        if (MovementAttempt(heightDifference, attemptedMoves, currentTerrain->connectedTerrain[i], targetTerrain, currentPath, true, bestCost) && attemptedMoves <= movesLeft) return true;
        currentPath.resize(savedPathSize);
        attemptedMoves = savedMoves;
    }

    return false;
}
