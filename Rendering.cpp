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
    int speed = 32;
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
}