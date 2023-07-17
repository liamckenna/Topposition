#include "MouseLogic.h"

void zoom(SDL_Event e, Input* playerInput) {

    std::pair<float, float> relativePositionB4;
    std::pair<float, float> newRelativePosition;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetResizable()) {
                if ((e.wheel.preciseY < 0 && gameObjects[i][j]->GetSize() <= 1) || (e.wheel.preciseY > 0 && gameObjects[i][j]->GetSize() >= 3.5)) {
                    continue;
                } else {

                    relativePositionB4.first = (gameObjects[i][j]->GetPosition().first - playerInput->currentMousePosition.first) / gameObjects[i][j]->GetSize();
                    relativePositionB4.second = (gameObjects[i][j]->GetPosition().second - playerInput->currentMousePosition.second) / gameObjects[i][j]->GetSize();
                    gameObjects[i][j]->AdjustSize((e.wheel.preciseY/abs(e.wheel.preciseY) * .1));
                    newRelativePosition.first = (gameObjects[i][j]->GetPosition().first - playerInput->currentMousePosition.first) / gameObjects[i][j]->GetSize();
                    newRelativePosition.second = (gameObjects[i][j]->GetPosition().second - playerInput->currentMousePosition.second) / gameObjects[i][j]->GetSize();

                    gameObjects[i][j]->SetPosition((gameObjects[i][j]->GetPosition().first + (relativePositionB4.first - newRelativePosition.first) * gameObjects[i][j]->GetSize()),
                                                   (gameObjects[i][j]->GetPosition().second + (relativePositionB4.second - newRelativePosition.second) * gameObjects[i][j]->GetSize()));
                    if (gameObjects[i][j]->GetSize() == 1 && i > 0) {
                        gameObjects[i][j]->SetDefaultPosition(gameObjects[i][j]->GetPosition().first, gameObjects[i][j]->GetPosition().second);
                    }
                }

            }

        }
    }
    RecenterScreen(playerInput);
}

void scroll(Input* playerInput) {
    bool enabled = true;
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (i == 0) {
                if (gameObjects[i][j]->GetSize() <= 1 ||
                    (gameObjects[i][j]->GetPosition().first > 0 && ((playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) > 0)) ||
                    ((gameObjects[i][j]->GetPosition().first + (gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetDimensions().first)) < SCREEN_WIDTH && ((playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) < 0)) ||
                    (gameObjects[i][j]->GetPosition().second > 0 && ((playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) > 0)) ||
                    ((gameObjects[i][j]->GetPosition().second + (gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetDimensions().second)) < SCREEN_HEIGHT && ((playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) < 0))) {
                    enabled = false;
                    continue;
                } else {
                    enabled = true;
                }
            }
            if (enabled && gameObjects[i][j]->GetResizable()){
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                               gameObjects[i][j]->GetPosition().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
            }
        }
    }

    RecenterScreen(playerInput);
}

GameObject* selectObject(int x, int y) {

    for (int i = gameObjects.size() - 1; i >= 0; i--) {
        for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
            int width_LowerBound = gameObjects[i][j]->GetPosition().first;
            int width_UpperBound = gameObjects[i][j]->GetBottomRight().first;
            int height_LowerBound = gameObjects[i][j]->GetPosition().second;
            int height_UpperBound = gameObjects[i][j]->GetBottomRight().second;

            if (x >= width_LowerBound && x <= width_UpperBound) {
                if (y >= height_LowerBound && y <= height_UpperBound) {

                    SDL_Color color = GetPixelColor(gameObjects[i][j]->GetSurface(),
                                                    (x - width_LowerBound)/(gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale()),
                                                    (y - height_LowerBound)/(gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    }
                    return gameObjects[i][j];

                }
            }

        }
    }
    return nullptr;
}

UIElement* selectUI(int x, int y) {
    for (int i = 0; i < uiElements.size(); i++) {
        int width_LowerBound = uiElements[i]->GetPosition().first;
        int width_UpperBound = uiElements[i]->GetBottomRight().first;
        int height_LowerBound = uiElements[i]->GetPosition().second;
        int height_UpperBound = uiElements[i]->GetBottomRight().second;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(uiElements[i]->GetSurface(),
                                                (x - width_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()),
                                                (y - height_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    continue;
                } else if (uiElements[i]->GetRendered()){
                    return uiElements[i];
                }


            }
        }

    }
    return nullptr;
}

Piece* selectPiece(int x, int y) {
    for (int i = pieces.size() - 1; i >= 0; i--) {
        if (!pieces[i]->GetSelectable() || !pieces[i]->GetRendered()) continue;
        int width_LowerBound = pieces[i]->GetPosition().first;
        int width_UpperBound = pieces[i]->GetBottomRight().first;
        int height_LowerBound = pieces[i]->GetPosition().second;
        int height_UpperBound = pieces[i]->GetBottomRight().second;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(pieces[i]->GetSurface(),
                                                (x - width_LowerBound)/(pieces[i]->GetSize() * pieces[i]->GetScale()),
                                                (y - height_LowerBound)/(pieces[i]->GetSize() * pieces[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    continue;
                } else {
                    Piece* temp = pieces[0];
                    pieces[0] = pieces[i];
                    pieces[i] = temp;
                    return pieces[0];
                }


            }
        }

    }
    return nullptr;
}

Terrain* selectTerrain(int x, int y) {
    for (int i = terrain.size() - 1; i >= 0; i--) {
        for (int j = terrain[i].size() - 1; j >= 0; j--) {
            int width_LowerBound = terrain[i][j]->GetPosition().first;
            int width_UpperBound = terrain[i][j]->GetBottomRight().first;
            int height_LowerBound = terrain[i][j]->GetPosition().second;
            int height_UpperBound = terrain[i][j]->GetBottomRight().second;

            if (x >= width_LowerBound && x <= width_UpperBound) {
                if (y >= height_LowerBound && y <= height_UpperBound) {

                    SDL_Color color = GetPixelColor(terrain[i][j]->GetSurface(),
                                                    (x - width_LowerBound)/(terrain[i][j]->GetSize() * terrain[i][j]->GetScale()),
                                                    (y - height_LowerBound)/(terrain[i][j]->GetSize() * terrain[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    }
                    return terrain[i][j];

                }
            }

        }
    }
    return nullptr;
}

Item* selectItem(int x, int y) {
    for (int i = 0; i < currentTurn->inventory.size(); i++) {
        int width_LowerBound = currentTurn->inventory[i]->GetPosition().first;
        int width_UpperBound = currentTurn->inventory[i]->GetBottomRight().first;
        int height_LowerBound = currentTurn->inventory[i]->GetPosition().second;
        int height_UpperBound = currentTurn->inventory[i]->GetBottomRight().second;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(currentTurn->inventory[i]->GetSurface(),
                                                (x - width_LowerBound)/(currentTurn->inventory[i]->GetSize() * currentTurn->inventory[i]->GetScale()),
                                                (y - height_LowerBound)/(currentTurn->inventory[i]->GetSize() * currentTurn->inventory[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    continue;
                } else {
                    return currentTurn->inventory[i];
                }


            }
        }

    }
    return nullptr;
}

void moveSelectedObject(GameObject* gameObject, Input* playerInput) {
    if (gameObject->GetMovable()) {
        gameObject->SetCenter(gameObject->GetCenter().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                              gameObject->GetCenter().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
        hoveringTerrain = selectTerrain(gameObject->GetCenter().first, gameObject->GetCenter().second + gameObject->GetDimensions().second/2 * gameObject->GetSize() * gameObject->GetScale());
    }
}

void RecenterScreen(Input* playerInput) {
    if (gameObjects[0][0]->GetPosition().first > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first - gameObjects[0][0]->GetPosition().first,
                                                   gameObjects[i][j]->GetPosition().second);
                }
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize() < SCREEN_WIDTH) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + (SCREEN_WIDTH - (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize())),
                                                   gameObjects[i][j]->GetPosition().second);
                }
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                                   gameObjects[i][j]->GetPosition().second - gameObjects[0][0]->GetPosition().second);
                }
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize() < SCREEN_HEIGHT) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                                   gameObjects[i][j]->GetPosition().second + (SCREEN_HEIGHT - (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize())));
                }
            }
        }
    }
}
