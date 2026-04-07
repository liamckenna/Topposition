#include "MouseLogic.h"
#include <cmath>

void zoom(SDL_MouseWheelEvent &event, Input *playerInput)
{
    float prevCameraZoom = cameraZoom;
    std::pair<float, float> relativePositionB4;
    std::pair<float, float> newRelativePosition;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    float mouseX = playerInput->currentMousePosition.first;
    float mouseY = playerInput->currentMousePosition.second;
    if (worldResolution.first > 9599.95f && worldResolution.first < 9600.05f)
    {
        if (event.y < 0)
        {
            return;
        }
        else
        {
            cameraZoom = std::ceil(cameraZoom * 10.0f) / 10.0f;
            if (cameraZoom == prevCameraZoom)
            {
                cameraZoom += 0.1f;
            }
        }
    }
    else if (worldResolution.first > 1919.95f && worldResolution.first < 1920.05f)
    {
        if (event.y > 0)
        {
            return;
        }
        else
        {
            cameraZoom = std::floor(cameraZoom * 10.0f) / 10.0f;
            if (cameraZoom == prevCameraZoom)
            {
                cameraZoom -= 0.1f;
            }
        }
    }
    else
    {
        cameraZoom += (event.y / abs(event.y) * .1);
        if (cameraZoom <= 0.f)
        {
            cameraZoom = SCREEN_WIDTH / 9600.f;
        }
        else if (SCREEN_WIDTH / cameraZoom > 9600.f)
        {
            cameraZoom = SCREEN_WIDTH / 9600.f;
        }
        else if (SCREEN_WIDTH / cameraZoom < 1920.f)
        {
            cameraZoom = SCREEN_WIDTH / 1920.f;
        }
    }
    std::cout << "Camera Zoom: " << cameraZoom << std::endl;

    cameraPosition.first += (mouseX / prevCameraZoom) - (mouseX / cameraZoom);
    cameraPosition.second += (mouseY / prevCameraZoom) - (mouseY / cameraZoom);
    //std::cout << "Camera Position: " << cameraPosition.first << ", " << cameraPosition.second << std::endl;

    worldResolution.first = SCREEN_WIDTH / cameraZoom;
    worldResolution.second = SCREEN_HEIGHT / cameraZoom;
    std::cout << "World Resolution: " << worldResolution.first << ", " << worldResolution.second << std::endl;

    ClampCameraBoundaries();
}

void scroll(Input *playerInput)
{
    cameraPosition.first -= (playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) / cameraZoom;
    cameraPosition.second -= (playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) / cameraZoom;


    //std::cout << "Camera Position: " << cameraPosition.first << ", " << cameraPosition.second << std::endl;

    ClampCameraBoundaries();
}

void ClampCameraBoundaries()
{
    std::pair<float, float> domain = {-2240, 7360};
    std::pair<float, float> range = {-1210, 4090};

    bool clamp = false;

    if (clamp)
    {
        if (cameraPosition.first < domain.first)
        {
            cameraPosition.first = domain.first;
        }
        else if (cameraPosition.first + worldResolution.first > domain.second)
        {
            cameraPosition.first = domain.second - worldResolution.first;
        }

        if (cameraPosition.second < range.first)
        {
            cameraPosition.second = range.first;
        }
        else if (cameraPosition.second + worldResolution.second > range.second)
        {
            cameraPosition.second = range.second - worldResolution.second;
        }
    }
}

GameObject *selectObject(int x, int y, bool update)
{
    for (int i = gameObjects.size() - 1; i >= 0; i--)
    {
        for (int j = gameObjects[i].size() - 1; j >= 0; j--)
        {
            int width_LowerBound = gameObjects[i][j]->GetPosition(update).first;
            int width_UpperBound = gameObjects[i][j]->GetBottomRight(update).first;
            int height_LowerBound = gameObjects[i][j]->GetPosition(update).second;
            int height_UpperBound = gameObjects[i][j]->GetBottomRight(update).second;

            if (x >= width_LowerBound && x <= width_UpperBound)
            {
                if (y >= height_LowerBound && y <= height_UpperBound)
                {

                    SDL_Color color = GetPixelColor(gameObjects[i][j]->GetSurface(),
                                                    (x - width_LowerBound) / (cameraZoom * gameObjects[i][j]->GetScale()),
                                                    (y - height_LowerBound) / (cameraZoom * gameObjects[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0)
                    {
                        continue;
                    }
                    return gameObjects[i][j];
                }
            }
        }
    }
    return nullptr;
}

UIElement *selectUI(int x, int y, bool update)
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (!uiElements[i]->GetSelectable())
        {
            continue;
        }
        int width_LowerBound, width_UpperBound, height_LowerBound, height_UpperBound;
        if (!uiElements[i]->GetResizable())
        {
            width_LowerBound = uiElements[i]->GetPosition(update).first;
            width_UpperBound = uiElements[i]->GetBottomRight(update).first;
            height_LowerBound = uiElements[i]->GetPosition(update).second;
            height_UpperBound = uiElements[i]->GetBottomRight(update).second;
        }
        else
        {
            width_LowerBound = uiElements[i]->GetPosition().first;
            width_UpperBound = uiElements[i]->GetBottomRight().first;
            height_LowerBound = uiElements[i]->GetPosition().second;
            height_UpperBound = uiElements[i]->GetBottomRight().second;
        }
        if (x >= width_LowerBound && x <= width_UpperBound)
        {
            if (y >= height_LowerBound && y <= height_UpperBound)
            {
                SDL_Color color = GetPixelColor(uiElements[i]->GetSurface(),
                                                (x - width_LowerBound) / (cameraZoom * uiElements[i]->GetScale()),
                                                (y - height_LowerBound) / (cameraZoom * uiElements[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0 && uiElements[i]->GetName() == "claim peak button")
                {
                    continue;
                }
                if (uiElements[i]->GetRendered())
                {
                    return uiElements[i];
                }
            }
        }
    }
    return nullptr;
}

Piece *selectPiece(int x, int y, bool update)
{
    for (int i = pieces.size() - 1; i >= 0; i--)
    {
        if (!pieces[i]->GetSelectable() || !pieces[i]->GetRendered())
            continue;
        int width_LowerBound = pieces[i]->GetPosition(update).first;
        int width_UpperBound = pieces[i]->GetBottomRight(update).first;
        int height_LowerBound = pieces[i]->GetPosition(update).second;
        int height_UpperBound = pieces[i]->GetBottomRight(update).second;
        if (x >= width_LowerBound && x <= width_UpperBound)
        {
            if (y >= height_LowerBound && y <= height_UpperBound)
            {

                SDL_Color color = GetPixelColor(pieces[i]->GetSurface(),
                                                (x - width_LowerBound) / (cameraZoom * pieces[i]->GetScale()),
                                                (y - height_LowerBound) / (cameraZoom * pieces[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0)
                {
                    continue;
                }
                else
                {
                    Piece *temp = pieces[0];
                    pieces[0] = pieces[i];
                    pieces[i] = temp;
                    return pieces[0];
                }
            }
        }
    }
    return nullptr;
}

Terrain *selectTerrain(int x, int y, bool update)
{
    for (int i = terrain.size() - 1; i >= 0; i--)
    {
        for (int j = terrain[i].size() - 1; j >= 0; j--)
        {
            int width_LowerBound = terrain[i][j]->GetPosition(update).first;
            int width_UpperBound = terrain[i][j]->GetBottomRight(update).first;
            int height_LowerBound = terrain[i][j]->GetPosition(update).second;
            int height_UpperBound = terrain[i][j]->GetBottomRight(update).second;

            if (x >= width_LowerBound && x <= width_UpperBound)
            {
                if (y >= height_LowerBound && y <= height_UpperBound)
                {

                    SDL_Color color = GetPixelColor(terrain[i][j]->GetSurface(),
                                                    (x - width_LowerBound) / (cameraZoom * terrain[i][j]->GetScale()),
                                                    (y - height_LowerBound) / (cameraZoom * terrain[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0)
                    {
                        continue;
                    }
                    return terrain[i][j];
                }
            }
        }
    }
    return nullptr;
}

Item *selectItem(int x, int y, bool update)
{
    for (int i = 0; i < currentTurn->inventory.size(); i++)
    {
        int width_LowerBound = currentTurn->inventory[i]->GetPosition(update).first;
        int width_UpperBound = currentTurn->inventory[i]->GetBottomRight(update).first;
        int height_LowerBound = currentTurn->inventory[i]->GetPosition(update).second;
        int height_UpperBound = currentTurn->inventory[i]->GetBottomRight(update).second;
        if (x >= width_LowerBound && x <= width_UpperBound)
        {
            if (y >= height_LowerBound && y <= height_UpperBound)
            {

                SDL_Color color = GetPixelColor(currentTurn->inventory[i]->GetSurface(),
                                                (x - width_LowerBound) / (cameraZoom * currentTurn->inventory[i]->GetScale()),
                                                (y - height_LowerBound) / (cameraZoom * currentTurn->inventory[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0)
                {
                    continue;
                }
                else
                {
                    return currentTurn->inventory[i];
                }
            }
        }
    }
    return nullptr;
}

void moveSelectedObject(GameObject *gameObject, Input *playerInput)
{
    if (gameObject->GetMovable())
    {
        gameObject->SetCenter(gameObject->GetCenter().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                              gameObject->GetCenter().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);

        Terrain *terrain_under = selectTerrain(gameObject->GetBottomMiddle().first, gameObject->GetBottomMiddle().second);
        if (terrain_under == nullptr) seaHover = true;
        else seaHover = false;
        if (hoveringTerrain != terrain_under)
        {
            hoveringTerrain = terrain_under;
            validMove = CheckMovementPossibility(dynamic_cast<Piece *>(gameObject), hoveringTerrain);
        }
    }
}
