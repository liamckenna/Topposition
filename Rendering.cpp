#include "Rendering.h"
#include "MultiPurposeFunctions.h"
void RenderScreen()
{
    if (seaHover)
    {
        if (validMove)
        {
            if (startingTerrain != nullptr)
            {
                SDL_SetRenderDrawColor(renderer, 2 / 1.5f, 120 / 1.5f, 150 / 1.5f, 100);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 2, 120, 150, 100);
            }
        }
        else
        {
            int r = 2;
            int g = 120;
            int b = 150;
            HueShift(r, g, b, -192.0f);
            SDL_SetRenderDrawColor(renderer, r, g, b, 100);
        }
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 2, 120, 150, 100);
    }
    SDL_RenderClear(renderer);
    // Render texture to screen
    if (state == MAIN_MENU)
    {
        renderUI();
    }
    else if (state == GAME)
    {
        renderOcean();
        renderTerrain();
        renderClaimNotifs();
        renderPieces();
        renderUI();
        renderText();
        //renderInventory();
    }

    // Update screen
    SDL_RenderPresent(renderer);
}

void renderUI()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetAssociatedPeak() == nullptr)
        {
            uiElements[i]->RenderGameObject(renderer);
        }
    }
}

void renderClaimNotifs()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetAssociatedPeak() != nullptr)
        {
            uiElements[i]->RenderGameObject(renderer);
        }
    }
}

void renderTerrain()
{
    for (int i = 0; i < terrain.size(); i++)
    {
        for (int j = 0; j < terrain[i].size(); j++)
        {
            terrain[i][j]->RenderGameObject(renderer, hoveringTerrain, validMove);
        }
    }
}

void renderPieces()
{
    for (int i = pieces.size() - 1; i >= 0; i--)
    {
        if (pieces[i]->type != GameObject::ITEM)
        {
            pieces[i]->RenderGameObject(renderer);
        }
    }
}

void renderInventory()
{
    for (int i = 0; i < currentTurn->inventory.size(); i++)
    {
        currentTurn->inventory[i]->RenderGameObject(renderer);
    }
}

void renderText()
{
    for (int i = 0; i < text.size(); i++)
    {
        text[i]->RenderText(renderer);
    }
}

void renderOcean()
{
    int speed = 128;
    if (deltaTime > 100)
        deltaTime = 5;
    for (int i = 0; i < ocean.size(); i++)
    {
        for (int j = 0; j < ocean[i].size(); j++)
        {
            float dxf = speed * deltaTime / 1000.f;
            float dyf = speed * deltaTime / 1000.f;

            ocean[i][j]->SetGlobalPosition(ocean[i][j]->globalPosition.first + dxf, ocean[i][j]->globalPosition.second + dyf);
            ocean[i][j]->CheckTimer();
            ocean[i][j]->RenderGameObject(renderer);
        }
    }
    CheckOceanTilePositioning();
}

void CheckOceanTilePositioning()
{
    std::vector<OceanTile *> edgeTiles;
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            if (ocean[i][j]->globalPosition.first >= 7680 || ocean[i][j]->globalPosition.second >= 6560)
            {
                edgeTiles.push_back(ocean[i][j]);
                std::cout << "added ocean tile \"" << ocean[i][j]->GetName() << "\" at position (" << ocean[i][j]->globalPosition.first << ", " << ocean[i][j]->globalPosition.second << ") to edgeTiles" << std::endl;
            }
        }
    }
    for (int i = 0; i < edgeTiles.size(); i++)
    {
        ReflectOceanTile(edgeTiles[i]);
    }
}

void ReflectOceanTile(OceanTile *tile)
{
    tile->SetGlobalPosition(tile->globalPosition.first + 1024, tile->globalPosition.second + 1024);
    float x = tile->globalPosition.first;
    float y = tile->globalPosition.second;

    float newX = 4000 - y;
    float newY = 4000 - x;

    tile->SetGlobalPosition(newX, newY);
    std::cout << "Reflected ocean tile \"" << tile->GetName()
              << "\" from old position (" << x << ", " << y
              << ") to new position (" << newX << ", " << newY << ")" << std::endl;
}