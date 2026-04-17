#include "GameObject.h"
#include "GlobalVars.h"
#include <iostream>

GameObject::GameObject(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r)
{
    this->name = name;
    this->texture = texture;
    this->surface = surface;
    SDL_GetTextureSize(texture, &dimensions.first, &dimensions.second);

    SetGlobalPosition(0, 0);
    movable = m;
    rendered = r;
}

void GameObject::RenderGameObject(SDL_Renderer *renderer)
{
    bool fetch_success = SDL_GetTextureSize(texture, &dimensions.first, &dimensions.second);

    if (topLayer)
    {
        renderRect->x = globalPosition.first;
        renderRect->y = globalPosition.second;
        renderRect->w = (float)(dimensions.first * scale);
        renderRect->h = (float)(dimensions.second * scale);
    }
    else
    {
        renderRect->x = (globalPosition.first - cameraPosition.first) * cameraZoom;
        renderRect->y = (globalPosition.second - cameraPosition.second) * cameraZoom;
        renderRect->w = (float)(dimensions.first * cameraZoom * scale);
        renderRect->h = (float)(dimensions.second * cameraZoom * scale);
    }
    
    if (rendered)
    {
        if (currentAnimation == nullptr)
        {
            if (type == OCEAN_TILE)
            {
                SDL_SetTextureAlphaMod(texture, static_cast<int>(dynamic_cast<OceanTile *>(this)->GetAlphaFloat() * 255));

                if (seaHover)
                {
                    if (validMove)
                    {
                        if (startingTerrain != nullptr)
                        {
                            SDL_SetTextureColorMod(texture, 255 / 2, 255 / 2, 255 / 2);
                        }
                    }
                    else
                    {
                        SDL_SetTextureColorMod(texture, 255, 0, 0);
                    }
                }

                SDL_RenderTextureRotated(renderer, texture, NULL, renderRect, dynamic_cast<OceanTile *>(this)->GetRotation(), NULL, SDL_FLIP_NONE);
                SDL_SetTextureAlphaMod(texture, static_cast<int>(255));

                if (seaHover)
                {
                    if (!validMove || startingTerrain != nullptr)
                    {
                        SDL_SetTextureColorMod(texture, 255, 255, 255);
                    }   
                }
            }
            else if (type == UI_ELEMENT)
            {
                UIElement *uiElement = dynamic_cast<UIElement *>(this);
                if (uiElement->GetRenderShadow())
                {
                    SDL_FRect *shadowRect = uiElement->GetShadowRect();
                    shadowRect->x = position.first + (uiElement->GetShadowOffsetX() * SCREEN_WIDTH / 3840.f);
                    shadowRect->y = position.second + (uiElement->GetShadowOffsetY() * SCREEN_HEIGHT / 2160.f);
                    shadowRect->w = dimensions.first * scale;
                    shadowRect->h = dimensions.second * scale;
                    if (uiElement->GetName() == "crown")
                    {
                        SDL_RenderTextureRotated(renderer, uiElement->GetShadowTexture(), NULL, shadowRect, uiElement->GetRotation(), NULL, SDL_FLIP_NONE);
                    }
                    else 
                    {
                        SDL_RenderTexture(renderer, uiElement->GetShadowTexture(), NULL, shadowRect);
                    }
                }
                if (uiElement->GetName() == "crown")
                {
                    SDL_RenderTextureRotated(renderer, texture, NULL, renderRect, uiElement->GetRotation(), NULL, SDL_FLIP_NONE);
                }
                else
                {
                    SDL_RenderTexture(renderer, texture, NULL, renderRect);
                }
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

pair<float, float> GameObject::GetPosition(bool update)
{
    if (update)
    {
        position.first = (globalPosition.first - cameraPosition.first) * cameraZoom;
        position.second = (globalPosition.second - cameraPosition.second) * cameraZoom;
    }
    return position;
}

pair<float, float> GameObject::GetBottomRight(bool update)
{
    if (update)
    {
        bottomRight.first = (globalPosition.first - cameraPosition.first + (dimensions.first * scale)) * cameraZoom;
        bottomRight.second = (globalPosition.second - cameraPosition.second + (dimensions.second * scale)) * cameraZoom;
    }
    return bottomRight;
}

pair<float, float> GameObject::GetBottomMiddle(bool update)
{
    if (update)
    {
        center.first = (globalPosition.first - cameraPosition.first + (dimensions.first * scale) / 2) * cameraZoom;
        bottomRight.second = (globalPosition.second - cameraPosition.second + (dimensions.second * scale)) * cameraZoom;
    }
    return std::make_pair(center.first, bottomRight.second);
}

void GameObject::SetGlobalPosition(float x, float y, bool updateRelative)
{
    globalPosition.first = x;
    globalPosition.second = y;

    if (updateRelative)
    {
        UpdateRelativePositions();
    }
    
}

pair<float, float> GameObject::GetGlobalCenter()
{
    return {globalPosition.first + (dimensions.first * scale / 2), globalPosition.second + (dimensions.second * scale / 2)};
}

void GameObject::SetGlobalCenter(float x, float y, bool updateRelative)
{
    globalPosition.first = x - (dimensions.first * scale / 2);
    globalPosition.second = y - (dimensions.second * scale / 2);

    if (updateRelative)
    {
        UpdateRelativePositions();
    }
}

void GameObject::UpdateRelativePositions()
{
    UpdatePosition();
    UpdateCenter();
    UpdateBottomRight();
}

void GameObject::UpdatePosition()
{
    if (topLayer)
    {
        position.first = globalPosition.first;
        position.second = globalPosition.second;
    }
    else
    {
        position.first = (globalPosition.first - cameraPosition.first) * cameraZoom;
        position.second = (globalPosition.second - cameraPosition.second) * cameraZoom;
    }
}

void GameObject::UpdateCenter()
{
    if (topLayer)
    {
        center.first = globalPosition.first + (dimensions.first * scale / 2);
        center.second = globalPosition.second + (dimensions.second * scale / 2);
    }
    else
    {
        center.first = (globalPosition.first - cameraPosition.first + (dimensions.first * scale / 2)) * cameraZoom;
        center.second = (globalPosition.second - cameraPosition.second + (dimensions.second * scale / 2)) * cameraZoom;
    }
}

void GameObject::UpdateBottomRight()
{
    if (topLayer)
    {
        bottomRight.first = globalPosition.first + (dimensions.first * scale);
        bottomRight.second = globalPosition.second + (dimensions.second * scale);
    }
    else
    {
        bottomRight.first = (globalPosition.first - cameraPosition.first + (dimensions.first * scale)) * cameraZoom;
        bottomRight.second = (globalPosition.second - cameraPosition.second + (dimensions.second * scale)) * cameraZoom;
    }    
}

void GameObject::UpdateGlobalPosition()
{
    if (topLayer)
    {
        globalPosition.first = position.first;
        globalPosition.second = position.second;
    }
    else
    {
        globalPosition.first = position.first / cameraZoom + cameraPosition.first;
        globalPosition.second = position.second / cameraZoom + cameraPosition.second;
    }
}

void GameObject::SetPosition(float x, float y, bool posOnly)
{
    position.first = x;
    position.second = y;

    UpdateGlobalPosition();

    if (!posOnly)
    {
        UpdateCenter();
        UpdateBottomRight();
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
        center.first = x;
        center.second = y;
        
        if (!centerOnly)
        {
            SetPosition(center.first - (dimensions.first * cameraZoom * scale) / 2, center.second - (dimensions.second * cameraZoom * scale) / 2, true);
            UpdateBottomRight();
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
        bottomRight.first = x;
        bottomRight.second = y;
        if (!brOnly)
        {
            SetPosition(bottomRight.first - (dimensions.first * cameraZoom * scale), bottomRight.second - (dimensions.second * cameraZoom * scale), true);
            UpdateCenter();
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
        center.first = x;
        bottomRight.second = y;

        if (!bmOnly)
        {
            SetPosition(center.first - (dimensions.first * cameraZoom * scale) / 2, bottomRight.second - (dimensions.second * cameraZoom * scale), true);
            UpdateCenter();
            UpdateBottomRight();
        }
    }
}

void Terrain::RenderGameObject(SDL_Renderer *renderer, Terrain *hoveringTerrain, bool validHover)
{
    bool hovering = false;
    bool isStartingTerrain = false;
    if (hoveringTerrain != nullptr)
    {
        if (this == hoveringTerrain)
        {
            hovering = true;
            if (this == startingTerrain)
            {
                isStartingTerrain = true;
            }
        }
        else if (this != nullptr)
        {
            for (int i = 0; i < connectedTerrain.size(); i++)
            {
                if (connectedTerrain[i] == hoveringTerrain)
                {
                    hovering = true;
                    if (connectedTerrain[i] == startingTerrain)
                    {
                        isStartingTerrain = true;
                    }
                    if (hovering && isStartingTerrain)
                    {
                        break;
                    }
                }
            }
        }
    }

    if (hovering)
    {
        if (validHover)
        {
            if (!isStartingTerrain)
            {
                SDL_SetTextureColorMod(pixels, 255 / 2, 255 / 2, 255 / 2);
            }
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
    else
    {
        unclaimedPeakCount--;
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

    renderRect->x = globalPosition.first - hiddenTerrain->GetPosition().first;
    renderRect->y = globalPosition.second - hiddenTerrain->GetPosition().second;
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

Text::Text(string n, const char *fp, SDL_Color c, int x, int y, int s, SDL_Renderer *r, const char *t)
{
    name = n;
    fontPath = fp;
    color = c;
    text = t;
    size = s;
    position.first = x;
    position.second = y;
    shadowOffsetX = 10;
    shadowOffsetY = 10;
    shadowAlpha = 150;
    font = TTF_OpenFont(fontPath, size);

    SDL_Color shadowColor = {0, 0, 0, shadowAlpha};
    SDL_Surface *shadowSurface = TTF_RenderText_Solid(font, text, 0, shadowColor);
    shadowTexture = SDL_CreateTextureFromSurface(r, shadowSurface);
    SDL_SetTextureAlphaMod(shadowTexture, shadowAlpha);
    SDL_DestroySurface(shadowSurface);

    surface = TTF_RenderText_Solid(font, text, 0, color);
    texture = SDL_CreateTextureFromSurface(r, surface);

    dimensions.first = surface->w;
    dimensions.second = surface->h;
}

void Text::RenderText(SDL_Renderer *renderer)
{
    if (rendered)
    {
        if (renderShadow)
        {
            shadowRect->x = position.first + (shadowOffsetX * SCREEN_WIDTH / 3840.f);
            shadowRect->y = position.second + (shadowOffsetY * SCREEN_HEIGHT / 2160.f);
            shadowRect->w = dimensions.first;
            shadowRect->h = dimensions.second;
            SDL_RenderTexture(renderer, shadowTexture, NULL, shadowRect);
        }
        
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