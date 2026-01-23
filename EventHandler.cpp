#include "EventHandler.h"
#include <future>

void HandleEvents(Input *playerInput)
{
    SDL_Event e;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_EVENT_QUIT:
            quit = true;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if (e.wheel.y > 0 || e.wheel.y < 0)
            {
                if (selectedObject == nullptr)
                {
                    zoom(e, playerInput);
                }
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            playerInput->MouseButtonDown(e.button);
            if (playerInput->GetMouseButtonDown("Left"))
            {
                SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                selectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                // if (selectedObject != nullptr) Print(selectedObject->GetName() + " ok");
                if (state == GAME)
                {
                    if (selectedObject == nullptr)
                    {
                        selectedObject = selectItem(playerInput->currentMousePosition.first,
                                                    playerInput->currentMousePosition.second);
                        if (selectedObject == nullptr)
                        {
                            selectedObject = selectPiece(playerInput->currentMousePosition.first,
                                                         playerInput->currentMousePosition.second);
                            if (selectedObject != nullptr && selectedObject->type == GameObject::PIECE)
                            {
                                Piece *piece = dynamic_cast<Piece *>(selectedObject);

                                piece->SetDesignatedLocation(piece->GetBottomMiddle().first, piece->GetBottomMiddle().second); // bottom middle of player
                                // std::cout << "Before:" << std::endl;
                                // std::cout << "Designated Location: (" << piece->GetDesignatedLocation().first << ", " << piece->GetDesignatedLocation().second << ")" << std::endl;
                                piece->SetScale(piece->GetScale() * 2);
                                piece->SetBottomMiddle(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
                                if (piece->GetCurrentAnimation() != NULL)
                                    piece->GetCurrentAnimation()->Pause();
                            }
                            else
                            {
                                // selectedObject = selectObject(gameObjects, playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                            }
                        }
                    }
                }
            }
            else if (playerInput->GetMouseButtonDown("Right"))
            {
                SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                std::cout << playerInput->currentMousePosition.first << ", " << playerInput->currentMousePosition.second << std::endl;
                // UpdateScore();
            }

            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (selectedObject != nullptr)
            {
                // std::cout << selectedObject->GetName() << " wow" << std::endl;
                if (state == MAIN_MENU)
                {
                    if (selectedObject->GetName() == "play")
                    {
                        std::cout << "hit play button" << std::endl;
                        state = GAME;
                        ResetMap();
                        break;
                    }
                    else if (selectedObject->GetName() == "settings")
                    {
                    }
                    else if (selectedObject->GetName() == "quit")
                    {
                        quit = true;
                        break;
                    }
                }
                else if (selectedObject->GetName() == "reset button")
                {
                    ResetMap();
                }
                else if ((selectedObject->GetName() == "dieOne" ||
                          selectedObject->GetName() == "dieTwo") &&
                         movesLeft < 1)
                {
                    currentRoll = Roll();
                    movesLeft = currentRoll;
                }
                else if ((selectedObject->GetName() == "dieOne"))
                {
                    // UpdateScore();
                }
                else if (selectedObject->GetName() == "finish turn button")
                {
                    FinishTurn();
                }
                else if (selectedObject->GetName() == "claim peak button")
                {
                    ClaimPeak(dynamic_cast<UIElement *>(selectedObject));

                    RefreshClaimNotifs();
                }
                else if (selectedObject->type == GameObject::PIECE)
                {
                    Piece *piece = dynamic_cast<Piece *>(selectedObject);
                    pair<float, float> bottom_middle = piece->GetBottomMiddle();
                    piece->SetScale(piece->GetScale() / 2.f);
                    piece->SetBottomMiddle(bottom_middle.first, bottom_middle.second);
                    Terrain *startingTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
                    Terrain *targetTerrain = selectTerrain(piece->GetBottomMiddle().first, piece->GetBottomMiddle().second);
                    if (piece->GetCurrentAnimation() != NULL)
                        piece->GetCurrentAnimation()->Unpause();
                    bool successful_move = Move(piece, startingTerrain, targetTerrain, movesLeft);
                    if (successful_move) // std::cout << "MOVE SUCCESS!" << std::endl;
                        // else //std::cout << "MOVE FAIL!" << std::endl;
                        // std::cout << "After:" << std::endl;
                        // std::cout << "Designated Location: (" << piece->GetDesignatedLocation().first << ", " << piece->GetDesignatedLocation().second << ")" << std::endl;
                        hoveringTerrain = nullptr;
                }
                else if (selectedObject->type == GameObject::ITEM)
                {
                    // std::cout << "item selected: " << selectedObject->GetName() << std::endl;
                }
            }
            playerInput->MouseButtonUp(e.button);
            selectedObject = nullptr;
            break;
        }
    }
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    if (playerInput->GetMouseButtonDown("Middle"))
    {
        scroll(playerInput);
    }
    if (playerInput->GetMouseButtonDown("Left"))
    {
        if (selectedObject != nullptr)
        {
            moveSelectedObject(selectedObject, playerInput);
        }
    }
    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}
