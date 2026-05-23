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
    switch (state)
    {   
        case MAIN_MENU:
            renderOcean();
            renderTerrain();
            renderPauseOverlay();
            renderUI();
            renderText();
            break;
        case GAME:
            renderOcean();
            renderTerrain();
            renderPieces();
            renderClaimNotifs();
            renderSelectedObject();
            renderUI();
            renderText();
            renderInventory();
            break;
        case PAUSED:
            renderOceanFrozen();
            renderTerrain();
            renderPieces();
            renderClaimNotifs();
            renderSelectedObject();
            renderUI();
            renderText();
            break;
        default:
            break;
    }
    
    SDL_RenderPresent(renderer);
}

void renderMenuBackground()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetName() == "background")
        {
            uiElements[i]->RenderGameObject(renderer);
        }
    }
}

void renderUI()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetAssociatedPeak() == nullptr && uiElements[i]->GetName() != "background")
        {
            if ((selectedObject == nullptr || uiElements[i] != selectedObject))
            {
                if (uiElements[i]->GetGameStateContext() == state || (uiElements[i]->GetGameStateContext() == GAME && state == PAUSED))
                {
                    uiElements[i]->RenderGameObject(renderer);
                }
            }

        }
    }
}

void renderClaimNotifs()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetAssociatedPeak() != nullptr)
        {
            if (selectedObject == nullptr || uiElements[i] != selectedObject)
            {
                uiElements[i]->RenderGameObject(renderer);
            }
        }
    }
}

void renderClaimFlags()
{
    for (int i = pieces.size() - 1; i >= 0; i--)
    {
        if (pieces[i]->type == FLAG)
        {
            pieces[i]->RenderGameObject(renderer);
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
    std::vector<Piece*> sorted(pieces.begin(), pieces.end());

    for (int i = 1; i < (int)sorted.size(); i++)
    {
        Piece* key = sorted[i];
        float keyY = key->globalPosition.second;
        int j = i - 1;
        while (j >= 0 && sorted[j]->globalPosition.second > keyY)
        {
            sorted[j + 1] = sorted[j];
            j--;
        }
        sorted[j + 1] = key;
    }

    for (Piece* p : sorted)
    {
        if (p->type != ITEM)
        {
            if (selectedObject == nullptr || p != selectedObject)
            {
                p->RenderGameObject(renderer);
            }
        }
    }
}

void renderSelectedObject()
{
    if (selectedObject != nullptr)
    {
        selectedObject->RenderGameObject(renderer);
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
        switch (state)
        {
            case MAIN_MENU:
                if (text[i]->GetGameStateContext() == MAIN_MENU)
                {
                    text[i]->RenderText(renderer);
                }
                break;
            case LOADING:
                if (text[i]->GetGameStateContext() == LOADING)
                {
                    text[i]->RenderText(renderer);
                }
                break;
            case GAME:
                if (text[i]->GetGameStateContext() == GAME)
                {
                    text[i]->RenderText(renderer);
                }
                break;
            case PAUSED:
                if (text[i]->GetGameStateContext() == GAME)
                {
                    text[i]->RenderText(renderer);
                }
                break;
            default:
                text[i]->RenderText(renderer);
                break;
        }
    }

    if (state == PAUSED)
    {
        renderPauseOverlay();
        for (int i = 0; i < text.size(); i++)
        {
            if (text[i]->GetGameStateContext() == PAUSED)
            {
                if (text[i]->GetPauseStateContext() == pState)
                {
                    text[i]->RenderText(renderer);
                }
            }
        }
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
    CheckOceanTilePositioning();
}

void renderOceanFrozen()
{
    for (int i = 0; i < ocean.size(); i++)
    {
        for (int j = 0; j < ocean[i].size(); j++)
        {
            ocean[i][j]->RenderGameObject(renderer);
        }
    }
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
            }
        }
    }
    for (int i = 0; i < edgeTiles.size(); i++)
    {
        ReflectOceanTile(edgeTiles[i]);
    }
}

void renderPauseOverlay()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlay = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void ReflectOceanTile(OceanTile *tile)
{
    tile->SetGlobalPosition(tile->globalPosition.first + 1024, tile->globalPosition.second + 1024);
    float x = tile->globalPosition.first;
    float y = tile->globalPosition.second;

    float newX = 4000 - y;
    float newY = 4000 - x;

    tile->SetGlobalPosition(newX, newY);
}

void RenderLoadingScreen()
{
    SDL_Color White = {255, 255, 255};

    int textSize = 250 * (SCREEN_WIDTH / 3840.f);
    Text *loadingText = new Text("loadingText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Loading...");
    loadingText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    loadingText->SetRenderShadow(false);
    loadingText->SetGameStateContext(LOADING);
    loadingText->SetRenderShadow(true);
    loadingText->SetShadowOffset(20, 20);
    SDL_SetRenderDrawColor(renderer, 2, 120, 150, 100);
    SDL_RenderClear(renderer);
    loadingText->RenderText(renderer);
    SDL_RenderPresent(renderer);
    free(loadingText);
}

void UpdateFPSCounter(int fps)
{
    if (fpsCounterText == nullptr)
        return;
    
    string fpsString = to_string(fps) + " FPS";

    float right = fpsCounterText->GetBottomRight().first;

    fpsCounterText->SetTextContent(fpsString.c_str(), renderer);
    fpsCounterText->SetPosition(right - fpsCounterText->GetWidth(), fpsCounterText->GetPosition().second);
}