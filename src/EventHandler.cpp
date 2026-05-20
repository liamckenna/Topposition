#include "EventHandler.h"
#include "MenuInitialization.h"
#include "MouseLogic.h"
#include <future>

void HandleEvents(Input *playerInput)
{
    SDL_Event event;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);

    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            EventQuit(playerInput, event.button);
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            EventWindowResized(playerInput, event.window);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            EventMouseWheel(playerInput, event.wheel);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            EventMouseButtonDown(playerInput, event.button);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            EventMouseButtonUp(playerInput, event.button);
            break;
        case SDL_EVENT_KEY_DOWN:
            EventKeyDown(playerInput, event.key);
            break;
        }
    }

    MouseMovement(playerInput);

    updateHoverState(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);

    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}

void EventQuit(Input *playerInput, SDL_MouseButtonEvent &event)
{
    quit = true;
}

void EventWindowResized(Input *playerInput, SDL_WindowEvent &event)
{
    float oldCameraZoom = cameraZoom;
    int oldScreenWidth = SCREEN_WIDTH;
    int oldScreenHeight = SCREEN_HEIGHT;
    SCREEN_WIDTH = event.data1;
    SCREEN_HEIGHT = event.data2;

    switch (state)
    {
    case SETTINGS:
        break;
    case LOADING:
        break;
    case MAIN_MENU:
    case GAME:
        if (SCREEN_WIDTH / cameraZoom > 9600.f)
        {
            cameraZoom = SCREEN_WIDTH / 9600.f;
        }
        else if (SCREEN_WIDTH / cameraZoom < 1920.f)
        {
            cameraZoom = SCREEN_WIDTH / 1920.f;
        }
        worldResolution.first = SCREEN_WIDTH / cameraZoom;
        worldResolution.second = SCREEN_HEIGHT / cameraZoom;
        cameraPosition.first = cameraPosition.first - (SCREEN_WIDTH / 2 / cameraZoom) + (oldScreenWidth / 2 / oldCameraZoom);
        cameraPosition.second = cameraPosition.second - (SCREEN_HEIGHT / 2 / cameraZoom) + (oldScreenHeight / 2 / oldCameraZoom);
        ClampCameraBoundaries();
        break;
    case PAUSED:
        if (SCREEN_WIDTH / cameraZoom > 9600.f)
        {
            cameraZoom = SCREEN_WIDTH / 9600.f;
        }
        else if (SCREEN_WIDTH / cameraZoom < 1920.f)
        {
            cameraZoom = SCREEN_WIDTH / 1920.f;
        }
        worldResolution.first = SCREEN_WIDTH / cameraZoom;
        worldResolution.second = SCREEN_HEIGHT / cameraZoom;
        cameraPosition.first = cameraPosition.first - (SCREEN_WIDTH / 2 / cameraZoom) + (oldScreenWidth / 2 / oldCameraZoom);
        cameraPosition.second = cameraPosition.second - (SCREEN_HEIGHT / 2 / cameraZoom) + (oldScreenHeight / 2 / oldCameraZoom);
        ClampCameraBoundaries();
        break;
    default:
        break;
    }

    std::cout << "New screen dimensions: " << SCREEN_WIDTH << ", " << SCREEN_HEIGHT << std::endl;
    updateUIElementPositions();
}

void EventKeyDown(Input *playerInput, SDL_KeyboardEvent &event)
{
    switch (state)
    {
    case MAIN_MENU:
        break;
    case SETTINGS:
        break;
    case LOADING:
        break;
    case GAME:
        if (selectedObject == nullptr && selectedText == nullptr && event.key == SDLK_ESCAPE)
        {
            state = PAUSED;
        }
        if (event.key == SDLK_F11)
        {
            RefreshShadows();
        }
        break;
    case PAUSED:
        if (event.key == SDLK_ESCAPE)
        {
            state = GAME;
        }
        break;
    default:
        break;
    }
}

void EventMouseWheel(Input *playerInput, SDL_MouseWheelEvent &event)
{
    switch (state)
    {
    case MAIN_MENU:
        break;
    case SETTINGS:
        break;
    case LOADING:
        break;
    case GAME:
        if (event.y > 0 || event.y < 0)
        {
            if (selectedObject == nullptr)
            {
                int direction = event.y > 0 ? 1 : -1;
                SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                std::pair<float, float> mousePos = playerInput->currentMousePosition;
                zoom(direction, mousePos);
            }
        }
        break;
    case PAUSED:
        break;
    default:
        break;
    }
}

void EventMouseButtonDown(Input *playerInput, SDL_MouseButtonEvent &event)
{
    playerInput->MouseButtonDown(event);
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    switch (state)
    {
    case MAIN_MENU:
        MouseButtonDownMainMenu(playerInput, event);
        break;
    case SETTINGS:
        break;
    case LOADING:
        break;
    case GAME:
        MouseButtonDownGame(playerInput, event);
        break;
    case PAUSED:
        MouseButtonDownPaused(playerInput, event);
        break;
    default:
        break;
    }
}

void EventMouseButtonUp(Input *playerInput, SDL_MouseButtonEvent &event)
{
    playerInput->MouseButtonUp(event);
    switch (state)
    {
    case MAIN_MENU:
        MouseButtonUpMainMenu(playerInput, event);
        break;
    case SETTINGS:
        break;
    case LOADING:
        break;
    case GAME:
        MouseButtonUpGame(playerInput, event);
        break;
    case PAUSED:
        MouseButtonUpPaused(playerInput, event);
        break;
    default:
        break;
    }
}

void MouseMovement(Input *playerInput)
{
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    switch (state)
    {
    case MAIN_MENU:
        break;
    case SETTINGS:
        break;
    case LOADING:
        break;
    case GAME:
        if (IsBattleSequenceActive())
        {
            break;
        }
        if ((playerInput->GetMouseButtonDown("Middle") || playerInput->GetMouseButtonDown("Right")) && !playerInput->GetMouseButtonDown("Left"))
        {
            if (selectedObject == nullptr && selectedText == nullptr)
            {
                scroll(playerInput);
            }
        }
        else if (playerInput->GetMouseButtonDown("Left"))
        {
            if (selectedObject != nullptr)
            {
                moveSelectedObject(selectedObject, playerInput);
            }
            else if (selectedText == nullptr)
            {
                scroll(playerInput);
            }
        }

        break;
    case PAUSED:
        break;
    default:
        break;
    }
}

void MouseButtonDownMainMenu(Input *playerInput, SDL_MouseButtonEvent &event)
{
    switch (event.button)
    {
    case SDL_BUTTON_LEFT:
        selectedText = selectText(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        if (selectedText == nullptr)
        {
            selectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        }
        break;
    case SDL_BUTTON_RIGHT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    default:
        break;
    }
}

void MouseButtonDownGame(Input *playerInput, SDL_MouseButtonEvent &event)
{
    if (IsBattleSequenceActive())
    {
        return;
    }

    switch (event.button)
    {
    case SDL_BUTTON_LEFT:
        if (IsBattleSequenceActive())
        {
            break;
        }
        selectedText = selectText(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        if (selectedText == nullptr)
        {
            selectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second, false);
            if (selectedObject == nullptr && !playerInput->GetMouseButtonDown("Middle"))
            {
                selectedObject = selectItem(playerInput->currentMousePosition.first,
                                            playerInput->currentMousePosition.second);
                if (selectedObject == nullptr)
                {
                    selectedObject = selectPiece(playerInput->currentMousePosition.first,
                                                 playerInput->currentMousePosition.second);
                    if (selectedObject != nullptr && selectedObject->type == PIECE)
                    {
                        Piece *piece = dynamic_cast<Piece *>(selectedObject);

                        piece->SetDesignatedLocation(piece->GetBottomMiddle().first, piece->GetBottomMiddle().second);
                        piece->SetScale(piece->GetScale() * 2);
                        piece->SetHeld(true);
                        piece->SetBottomMiddle(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
                        startingTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
                        if (piece->GetCurrentAnimation() != NULL)
                            piece->GetCurrentAnimation()->Pause();
                        RefreshClaimNotifs();
                    }
                }
            }
        }
        break;
    case SDL_BUTTON_RIGHT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    default:
        break;
    }
}

void MouseButtonDownPaused(Input *playerInput, SDL_MouseButtonEvent &event)
{
    switch (event.button)
    {
    case SDL_BUTTON_LEFT:
        selectedText = selectText(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        break;
    case SDL_BUTTON_RIGHT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    default:
        break;
    }
}

void MouseButtonUpMainMenu(Input *playerInput, SDL_MouseButtonEvent &event)
{
    switch (event.button)
    {
    case SDL_BUTTON_LEFT:
    {
        Text *newSelectedText = selectText(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        if (selectedText != nullptr && selectedText == newSelectedText)
        {
            if (selectedText->GetName() == "playButtonText")
            {
                std::cout << "hit play button" << std::endl;
                RefreshShadows();
                state = GAME;
                //ResetMap();
            }
            else if (selectedText->GetName() == "settings")
            {
            }
            else if (selectedText->GetName() == "quitButtonText")
            {
                quit = true;
            }
        }
        if (selectedText != nullptr)
        {
            selectedText->SetSelected(false);
        }
        if (newSelectedText != nullptr)
        {
            newSelectedText->SetSelected(false);
        }
        newSelectedText = nullptr;
        selectedText = nullptr;
        break;
    }
    case SDL_BUTTON_RIGHT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    default:
        break;
    }
}

void MouseButtonUpGame(Input *playerInput, SDL_MouseButtonEvent &event)
{
    if (IsBattleSequenceActive())
    {
        
        if (selectedText != nullptr)
        {
            selectedText->SetSelected(false);
            if (selectedText == endText)
            {
                turnText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
            else if (selectedText == turnText)
            {
                endText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
        }
        if (selectedObject != nullptr)
        {
            if (selectedObject->type == UI_ELEMENT)
            {
                UIElement *uiElement = dynamic_cast<UIElement *>(selectedObject);
                uiElement->SetSelected(false);
                if (uiElement == endTurnArrow)
                {
                    endText->SetSelected(false);
                    turnText->SetSelected(false);
                }
            }
        }
        selectedText = nullptr;
        selectedObject = nullptr;
        return;
    }

    switch (event.button)
    {
    case SDL_BUTTON_LEFT:
    {
        Text *newSelectedText = selectText(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        if (selectedText != nullptr && selectedText == newSelectedText)
        {
            if (selectedText->GetName() == "turnText" || selectedText->GetName() == "endText")
            {
                FinishTurn();
            }
        }
        else if (selectedObject != nullptr)
        {
            GameObject *newSelectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second, false);
            if (selectedObject == newSelectedObject)
            {
                if ((selectedObject->GetName() == "dieOne" ||
                     selectedObject->GetName() == "dieTwo") &&
                    movesLeft < 1)
                {
                    currentRoll = Roll();
                    movesLeft = currentRoll;
                }
                else if (selectedObject->GetName() == "endTurnArrow")
                {
                    FinishTurn();
                }
                else if (selectedObject->GetName() == "claim peak button")
                {
                    ClaimPeak(dynamic_cast<UIElement *>(selectedObject));
                }
                else if (selectedObject->type == ITEM)
                {
                }
            }
            else if (selectedObject->type == PIECE)
            {
                Piece *piece = dynamic_cast<Piece *>(selectedObject);
                pair<float, float> bottom_middle = piece->GetBottomMiddle();
                piece->SetScale(piece->GetScale() / 2.f);
                piece->SetBottomMiddle(bottom_middle.first, bottom_middle.second);
                piece->SetHeld(false);
                // Terrain *startingTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
                Terrain *targetTerrain = selectTerrain(piece->GetBottomMiddle().first, piece->GetBottomMiddle().second);
                if (piece->GetCurrentAnimation() != NULL)
                    piece->GetCurrentAnimation()->Unpause();
                bool successful_move = Move(piece, startingTerrain, targetTerrain, movesLeft);
                RefreshClaimNotifs();
                startingTerrain = nullptr;
                hoveringTerrain = nullptr;
                seaHover = false;
                validMove = true;
            }
            newSelectedObject = nullptr;
        }
        if (selectedText != nullptr)
        {
            selectedText->SetSelected(false);
            if (selectedText == endText)
            {
                turnText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
            else if (selectedText == turnText)
            {
                endText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
        }
        if (newSelectedText != nullptr)
        {
            newSelectedText->SetSelected(false);
            if (newSelectedText == endText)
            {
                turnText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
            else if (newSelectedText == turnText)
            {
                endText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
        }
        if (selectedObject != nullptr && selectedObject->type == UI_ELEMENT)
        {
            UIElement *uiElement = dynamic_cast<UIElement *>(selectedObject);
            uiElement->SetSelected(false);
            if (uiElement == endTurnArrow)
            {
                endText->SetSelected(false);
                turnText->SetSelected(false);
            }
        }
        selectedText = nullptr;
        newSelectedText = nullptr;
        selectedObject = nullptr;
        break;
    }
    case SDL_BUTTON_RIGHT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    default:
        break;
    }
}

void MouseButtonUpPaused(Input *playerInput, SDL_MouseButtonEvent &event)
{
    switch (event.button)
    {
    case SDL_BUTTON_LEFT:
    {
        std::cout << "Mouse button up in paused state" << std::endl;
        Text *newSelectedText = selectText(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
        if (selectedText != nullptr && selectedText == newSelectedText)
        {
            if (selectedText == resetMapText)
            {
                ResetMap();
            }
            else if (selectedText == exitToMainMenuText)
            {
                LoadMenu();
            }
        }
        if (selectedText != nullptr)
        {
            selectedText->SetSelected(false);
            if (selectedText == endText)
            {
                turnText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
            else if (selectedText == turnText)
            {
                endText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
        }
        if (newSelectedText != nullptr)
        {
            newSelectedText->SetSelected(false);
            if (newSelectedText == endText)
            {
                turnText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
            else if (newSelectedText == turnText)
            {
                endText->SetSelected(false);
                endTurnArrow->SetSelected(false);
            }
        }
        newSelectedText = nullptr;
        selectedText = nullptr;
        break;
    }
    case SDL_BUTTON_RIGHT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    default:
        break;
    }
}