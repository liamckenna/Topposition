#include "GameObject.h"
#include "GlobalVars.h"
#include <iostream>

GameObject::GameObject(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r)
{
    this->name = name;
    this->texture = texture;
    this->surface = surface;
    SDL_GetTextureSize(texture, &dimensions.first, &dimensions.second);

    SetPosition(0, 0, true);
    SetCenter();
    movable = m;
    rendered = r;
}

void GameObject::RenderGameObject(SDL_Renderer *renderer)
{


    bool fetch_success = SDL_GetTextureSize(texture, &dimensions.first, &dimensions.second);

    if (resizable)
    {
        renderRect->x = (globalPosition.first - cameraPosition.first) * cameraZoom;
        renderRect->y = (globalPosition.second - cameraPosition.second) * cameraZoom;
        renderRect->w = (float)(dimensions.first * cameraZoom * scale);
        renderRect->h = (float)(dimensions.second * cameraZoom * scale);
    }
    else
    {
        renderRect->x = globalPosition.first;
        renderRect->y = globalPosition.second;
        renderRect->w = (float)(dimensions.first * scale);
        renderRect->h = (float)(dimensions.second * scale);
    }
    
    if (rendered)
    {
        if (currentAnimation == nullptr)
        {
            if (type == OCEAN_TILE)
            {
                SDL_SetTextureAlphaMod(texture, static_cast<int>(dynamic_cast<OceanTile *>(this)->GetAlphaFloat() * 255));
                SDL_RenderTextureRotated(renderer, texture, NULL, renderRect, dynamic_cast<OceanTile *>(this)->GetRotation(), NULL, SDL_FLIP_NONE);
                SDL_SetTextureAlphaMod(texture, static_cast<int>(255));
            }
            else
            {
                SDL_RenderTexture(renderer, texture, NULL, renderRect);
            }
        }
        else
        {
            SDL_RenderTexture(renderer, currentAnimation->GetSpriteSheet(), currentAnimation->GetRect(), renderRect);
        }
    }
}

void GameObject::SetPosition(float x, float y, bool posOnly)
{    
    position.first = x;
    position.second = y;
    if (!posOnly)
    {
        SetCenter();
        SetBottomRight();
    }
}

void GameObject::SetCenter(float x, float y, bool centerOnly)
{
    if (x == 0 && y == 0)
    {
        center.first = position.first + (dimensions.first * cameraZoom * scale) / 2;
        center.second = position.second + (dimensions.second * cameraZoom * scale) / 2;
    }
    else
    {
        if (!centerOnly)
        {
            center.first = x;
            center.second = y;
            position.first = center.first - (dimensions.first * cameraZoom * scale) / 2;
            position.second = center.second - (dimensions.second * cameraZoom * scale) / 2;

            SetBottomRight();
        }
        else
        {
            center.first = x;
            center.second = y;
        }
    }
}

void GameObject::SetRendered(bool r)
{
    rendered = r;
}

void GameObject::SetMovable(bool m)
{
    movable = m;
}

void GameObject::SetBottomRight(float x, float y, bool brOnly)
{

    if (x == 0 && y == 0)
    {
        bottomRight.first = position.first + (dimensions.first * cameraZoom * scale);
        bottomRight.second = position.second + (dimensions.second * cameraZoom * scale);
    }
    else
    {
        if (!brOnly)
        {
            bottomRight.first = x;
            bottomRight.second = y;
            position.first = bottomRight.first - (dimensions.first * cameraZoom * scale);
            position.second = bottomRight.second - (dimensions.second * cameraZoom * scale);

            SetCenter();
        }
        else
        {
            bottomRight.first = x;
            bottomRight.second = y;
        }
    }
}

void GameObject::SetBottomMiddle(float x, float y, bool bmOnly)
{

    if (x == 0 && y == 0)
    {
        center.first = position.first + (dimensions.first * cameraZoom * scale) / 2;
        bottomRight.second = position.second + (dimensions.second * cameraZoom * scale);
    }
    else
    {
        if (!bmOnly)
        {
            center.first = x;
            bottomRight.second = y;
            position.first = center.first - (dimensions.first * cameraZoom * scale) / 2;
            position.second = bottomRight.second - (dimensions.second * cameraZoom * scale);

            globalPosition.first = (position.first / cameraZoom) + cameraPosition.first;
            globalPosition.second = (position.second / cameraZoom) + cameraPosition.second;

            SetCenter();
            SetBottomRight();
        }
        else
        {
            center.first = x;
            bottomRight.second = y;
        }
    }
}

void Terrain::RenderGameObject(SDL_Renderer *renderer, Terrain *hoveringTerrain, bool validHover)
{
    bool hovering = false;
    if (hoveringTerrain != nullptr)
    {
        if (this == hoveringTerrain)
        {
            hovering = true;
        }
        else if (this != nullptr)
        {
            for (int i = 0; i < connectedTerrain.size(); i++)
            {
                if (connectedTerrain[i] == hoveringTerrain)
                {
                    hovering = true;
                    break;
                }
            }
        }
    }
    if (hovering)
    {
        if (validHover)
        {
            SDL_SetTextureColorMod(pixels, 255 / 2, 255 / 2, 255 / 2);
        }
        else
        {
            SDL_SetTextureColorMod(pixels, 255, 0, 0);
        }
    }

    renderRect->x = (globalPosition.first - cameraPosition.first) * cameraZoom;
    renderRect->y = (globalPosition.second - cameraPosition.second) * cameraZoom;

    renderRect->w = ceil(dimensions.first * cameraZoom * 2);
    renderRect->h = ceil(dimensions.second * cameraZoom * 2);
    if (rendered)
    {
        SDL_RenderTextureRotated(renderer, pixels, NULL, renderRect, 0, NULL, SDL_FLIP_NONE);
    }

    if (hovering)
    {
        SDL_SetTextureColorMod(pixels, 255, 255, 255);
    }
}

void Peak::Claim(Player *player)
{
    if (claimedBy != nullptr)
    {
        for (int i = 0; i < claimedBy->peaks.size(); i++)
        {
            if (claimedBy->peaks[i] == this)
            {
                claimedBy->peaks.erase(claimedBy->peaks.begin() + i);
                break;
            }
        }
    }
    claimedBy = player;
    player->peaks.push_back(this);
}

void Pixel::RenderGameObject(SDL_Renderer *renderer, Terrain *hoveringTerrain)
{

    SDL_SetRenderTarget(renderer, hiddenTerrain->GetPixels());

    bool hovering = false;
    if (hoveringTerrain != nullptr)
    {
        if (hiddenTerrain == hoveringTerrain)
        {
            hovering = true;
        }
        else if (this != nullptr)
        {
            for (int i = 0; i < hiddenTerrain->connectedTerrain.size(); i++)
            {
                if (hiddenTerrain->connectedTerrain[i] == hoveringTerrain)
                {
                    hovering = true;
                    break;
                }
            }
        }
    }
    if (hovering)
    {
        SDL_SetTextureColorMod(texture, color.r / 2, color.g / 2, color.b / 2);
    }
    else
    {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }

    renderRect->x = position.first - hiddenTerrain->GetPosition().first;
    renderRect->y = position.second - hiddenTerrain->GetPosition().second;
    renderRect->w = (width + 2);
    renderRect->h = (height + 2);
    SDL_RenderTexture(renderer, texture, NULL, renderRect);
    if (hovering)
    {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }
}

void OceanTile::CheckTimer()
{
    this->tileTime += deltaTime;

    if (fadingOut)
    {
        FadeOut();
    }
    else if (waitingToReturn)
    {
        if (tileTime >= (2 * 1000))
        {
            waitingToReturn = false;
            tileTime = 0;
            FadeIn();
        }
    }
    else if (fadingIn)
    {
        FadeIn();
    }
    else if (tileTime >= (lifespan * 1000))
    {
        FadeOut();
    }
}

void OceanTile::FadeOut()
{
    if (!fadingOut)
    {
        fadingOut = true;
    }

    if (alpha_float > 0.f)
    {
        alpha_float -= (0.25f * deltaTime) / 1000.f;
        if (alpha_float < 0.f)
            alpha_float = 0.f;
    }
    else
    {
        alpha_float = 0.f;
        FullFade();
    }
}

void OceanTile::FullFade()
{
    fadingOut = false;
    tileTime = 0;
    int index = rand() % 20 + 1;
    SetTexture(textures["tile " + to_string(index)][0]);
    SetSurface(surfaces["tile " + to_string(index)]);
    waitingToReturn = true;
}

void OceanTile::FadeIn()
{
    if (!fadingIn)
    {
        fadingIn = true;
        lifespan = rand() % 15 + 5;
    }

    if (alpha_float < 1.f)
    {
        alpha_float += (0.25f * deltaTime) / 1000.f;
        if (alpha_float > 1.f)
            alpha_float = 1.f;
    }
    else
    {
        fadingIn = false;
        tileTime = 0;
        alpha_float = 1.f;
    }
}

Text::Text(string n, const char *fp, SDL_Color c, int x, int y, int w, int h, int s, SDL_Renderer *r, const char *t)
{
    name = n;
    fontPath = fp;
    color = c;
    text = t;
    size = s;
    position.first = x;
    position.second = y;
    dimensions.first = w;
    dimensions.second = h;
    font = TTF_OpenFont(fontPath, size);
    surface = TTF_RenderText_Solid(font, text, 0, color);
    texture = SDL_CreateTextureFromSurface(r, surface);
}

void Text::RenderText(SDL_Renderer *renderer)
{
    if (rendered)
    {
        rect->x = position.first;
        rect->y = position.second;
        rect->w = dimensions.first;
        rect->h = dimensions.second;
        SDL_RenderTexture(renderer, texture, NULL, rect);
    }
}

Animation::Animation(SDL_Texture *ss, SDL_Surface *s, float d, int fc, pair<int, int> sd, pair<int, int> spd)
{
    spriteSheet = ss;
    surface = s;
    duration = d;
    frameCount = fc;
    sheetDimensions = sd;
    spriteDimensions = spd;
    frameOffset = rand() % frameCount;
    lastFrame = frameOffset;
}

void Animation::CycleFrame(Uint64 current)
{
    if (paused)
        lastUpdate = currentTime;
    float dT = (currentTime - lastUpdate) / 1000.0f;

    int framesToUpdate = floor(dT * frameCount / duration);
    if (framesToUpdate > 0)
    {
        lastFrame += framesToUpdate;
        lastFrame %= frameCount;
        lastUpdate = currentTime;
        int row = lastFrame / sheetDimensions.first;
        int column = lastFrame % sheetDimensions.first;
        rect->x = spriteDimensions.first * column;
        rect->y = spriteDimensions.second * row;
        rect->w = spriteDimensions.first;
        rect->h = spriteDimensions.second;
    }
}