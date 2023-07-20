#include "EventHandler.h"

void HandleEvents(Input* playerInput) {
    SDL_Event e;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    while( SDL_PollEvent( &e ) != 0 )
    {
        switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEWHEEL:
                if (e.wheel.preciseY > 0 || e.wheel.preciseY < 0) {
                    if (selectedObject == nullptr) {
                        zoom(e, playerInput);
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                playerInput->MouseButtonDown(e.button);
                if (playerInput->GetMouseButtonDown("Left")) {
                    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                    selectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                    if (selectedObject == nullptr) {
                        selectedObject = selectItem(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                        if (selectedObject == nullptr) {
                            selectedObject = selectPiece(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                            if (selectedObject != nullptr && selectedObject->type == GameObject::PIECE) {
                                Piece* piece = dynamic_cast<Piece *>(selectedObject);
                                int centerX = piece->GetCenter().first;
                                int centerY = piece->GetCenter().second;
                                piece->SetScale(piece->GetScale()*2);
                                piece->SetCenter(centerX, centerY - (piece->GetDimensions().second/2) * piece->GetSize() * (piece->GetScale()/2));
                                piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetCenter().second);
                                if (piece->GetCurrentAnimation() != NULL) piece->GetCurrentAnimation()->Pause();
                            } else {
                                //selectedObject = selectObject(gameObjects, playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                            }
                        }
                    }
                } else if (playerInput->GetMouseButtonDown("Right")) {
                    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                    std::cout << playerInput->currentMousePosition.first << ", " << playerInput->currentMousePosition.second << std::endl;
                }

                break;
            case SDL_MOUSEBUTTONUP:
                if (selectedObject != nullptr) {
                    if (selectedObject->GetName() == "reset button") {
                        ResetMap();
                    } else if ((selectedObject->GetName() == "dieOne" ||
                                selectedObject->GetName() == "dieTwo") && movesLeft < 1) {
                        currentRoll = Roll();
                        movesLeft = currentRoll;
                    } else if (selectedObject->GetName() == "finish turn button") {
                        FinishTurn();

                    } else if (selectedObject->GetName() == "claim peak button") {
                        ClaimPeak(dynamic_cast<UIElement *>(selectedObject));
                        RefreshClaimNotifs();

                    }else if (selectedObject->type == GameObject::PIECE) {
                        Piece* piece = dynamic_cast<Piece *>(selectedObject);
                        int centerX = piece->GetCenter().first;
                        int centerY = piece->GetCenter().second;


                        Terrain* startingTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second + (piece->GetDimensions().second/2) * piece->GetSize() * piece->GetScale());
                        Terrain* targetTerrain = selectTerrain(piece->GetCenter().first, centerY + (piece->GetDimensions().second/2) * piece->GetSize() * piece->GetScale());
                        piece->SetScale(piece->GetScale()*0.5f);
                        piece->SetCenter(centerX, centerY + (piece->GetDimensions().second/2) * piece->GetSize() * piece->GetScale());
                        if (piece->GetCurrentAnimation() != NULL) piece->GetCurrentAnimation()->Unpause();
                        Move(piece, startingTerrain, targetTerrain, movesLeft);
                        hoveringTerrain = nullptr;

                    } else if (selectedObject->type == GameObject::ITEM) {
                        std::cout << "item selected: " << selectedObject->GetName() << std::endl;
                    }
                }
                playerInput->MouseButtonUp(e.button);
                selectedObject = nullptr;
                break;
        }
    }
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    if (playerInput->GetMouseButtonDown("Middle")) {
        scroll(playerInput);
    }
    if (playerInput->GetMouseButtonDown("Left")) {
        if (selectedObject != nullptr) {
            moveSelectedObject(selectedObject, playerInput);
        }
    }
    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}
