#include "GameObject.h"
#include <iostream>

GameObject::GameObject(string name, SDL_Texture *texture, SDL_Surface* surface, bool m, bool r) {
    this->name = name;
    this->texture = texture;
    this->surface = surface;
    size = 1;
    SDL_QueryTexture(texture, nullptr, nullptr, &dimensions.first, &dimensions.second);

    SetPosition(0,0, true);
    SetCenter();
    SetDefaultPosition(0, 0);
    movable = m;
    rendered = r;
}

void GameObject::RenderGameObject(SDL_Renderer* renderer) {
    if (size == 1) {
        renderRect->x = defaultPosition.first;
        renderRect->y = defaultPosition.second;
    } else {
        renderRect->x = position.first;
        renderRect->y = position.second;
    }
    SDL_QueryTexture(texture, nullptr, NULL, &dimensions.first, &dimensions.second);
    //std::cout << dimensions.first << ", " << dimensions.second << std::endl;

    renderRect->w = (float) (dimensions.first * size * scale);
    renderRect->h = (float) (dimensions.second * size * scale);
    if (rendered) {
        if (currentAnimation == nullptr) {
            SDL_RenderCopy(renderer, texture, NULL, renderRect);
        } else {
            SDL_RenderCopy(renderer, currentAnimation->GetSpriteSheet(), currentAnimation->GetRect(), renderRect);
        }
    }
}

void GameObject::SetPosition(float x, float y, bool posOnly) {
    position.first = x;
    position.second = y;
    if (!posOnly) {
        SetCenter();
        SetBottomRight();
    }
    if (size == 1) {
        defaultPosition.first = position.first;
        defaultPosition.second = position.second;
    }
}



void GameObject::AdjustSize(float multiplier, int w, int h) {

    if (w == 0 || h == 0) {
        size += multiplier;
    } else {
        dimensions.first = w;
        dimensions.second = h;
    }
    SetPosition(center.first - (dimensions.first * size * scale) / 2,center.second - (dimensions.second * size * scale) / 2, true);
}
void GameObject::SetCenter(float x, float y, bool centerOnly){
    if (x == 0 && y == 0) {
        center.first = position.first + (dimensions.first * size * scale)/2;
        center.second = position.second + (dimensions.second * size * scale)/2;
    }
    else {
        if (!centerOnly) {
            center.first = x;
            center.second = y;
            position.first = center.first - (dimensions.first * size * scale) / 2;
            position.second = center.second - (dimensions.second * size * scale) / 2;

            SetBottomRight();
            if (size == 1) SetDefaultPosition(position.first, position.second);
        } else {
            center.first = x;
            center.second = y;
        }
    }

}

void GameObject::SetRendered(bool r) {
    rendered = r;
}

void GameObject::SetMovable(bool m) {
    movable = m;
}

void GameObject::SetDefaultPosition(float x, float y) {
    defaultPosition.first = x;
    defaultPosition.second = y;
}



void GameObject::SetBottomRight() {
    bottomRight.first = position.first + (dimensions.first * size * scale);
    bottomRight.second = position.second + (dimensions.second * size * scale);
}

void Terrain::RenderGameObject(SDL_Renderer *renderer, Terrain* hoveringTerrain) {
    bool hovering = false;
    if (hoveringTerrain != nullptr) {
        if (this == hoveringTerrain) {
            hovering = true;
        } else if (this != nullptr) {
            for (int i = 0; i < connectedTerrain.size(); i++) {
                if (connectedTerrain[i] == hoveringTerrain) {
                    hovering = true;
                    break;
                }
            }
        }
    }
    if (hovering) {
        SDL_SetTextureColorMod(pixels, 255/2, 255/2, 255/2);
    } else {
        //SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }
    if (size == 1) {
        renderRect->x = defaultPosition.first;
        renderRect->y = defaultPosition.second;
    } else {
        renderRect->x = position.first;
        renderRect->y = position.second;
    }
    renderRect->w = (dimensions.first * size * scale);
    renderRect->h = (dimensions.second * size * scale);
    SDL_RenderCopyEx( renderer, texture, NULL, renderRect, 0, NULL, SDL_FLIP_NONE);

    renderRect->w = (dimensions.first * size);
    renderRect->h = (dimensions.second * size);
    if (rendered) {
        SDL_RenderCopyEx( renderer, pixels, NULL, renderRect, 0, NULL, SDL_FLIP_NONE);
    }


    if (hovering) {
        SDL_SetTextureColorMod(pixels, 255, 255, 255);
    }
}

void Pixel::RenderGameObject(SDL_Renderer *renderer, Terrain* hoveringTerrain) {


    SDL_SetRenderTarget(renderer, hiddenTerrain->GetPixels());

    bool hovering = false;
    if (hoveringTerrain != nullptr) {
        if (hiddenTerrain == hoveringTerrain) {
            hovering = true;
        } else if (this != nullptr) {
            for (int i = 0; i < hiddenTerrain->connectedTerrain.size(); i++) {
                if (hiddenTerrain->connectedTerrain[i] == hoveringTerrain) {
                    hovering = true;
                    break;
                }
            }
        }
    }
    if (hovering) {
        SDL_SetTextureColorMod(texture, color.r/2, color.g/2, color.b/2);
    } else {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }

    if (size == 1) {
        renderRect->x = position.first - hiddenTerrain->GetPosition().first;
        renderRect->y = position.second - hiddenTerrain->GetPosition().second;
    } else {
        renderRect->x = position.first - hiddenTerrain->GetPosition().first;
        renderRect->y = position.second - hiddenTerrain->GetPosition().second;
    }
    renderRect->w = (width);
    renderRect->h = (height);

    SDL_RenderCopy(renderer, texture, NULL, renderRect);
    if (hovering) {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }
}

Text::Text(string n, const char* fp, SDL_Color c, int x, int y, int w, int h, int s, SDL_Renderer* r, const char* t) {
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
    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(r, surface);
}

void Text::RenderText(SDL_Renderer* renderer) {
    if (rendered) {
        rect->x = position.first;
        rect->y = position.second;
        rect->w = dimensions.first;
        rect->h = dimensions.second;
        SDL_RenderCopy(renderer, texture, NULL, rect);
    }
}

Animation::Animation(SDL_Texture *ss, SDL_Surface *s, float d, int fc, pair<int, int> sd, pair<int, int> spd) {
    spriteSheet = ss;
    surface = s;
    duration = d;
    frameCount = fc;
    sheetDimensions = sd;
    spriteDimensions = spd;
    frameOffset = rand() % frameCount;
    lastFrame = frameOffset;
}

void Animation::CycleFrame(Uint64 current) {
    if (paused) lastUpdate = current;
    float dT = (current - lastUpdate) / 1000.0f;


    int framesToUpdate = floor(dT / (1.0f / (frameCount/duration)));
    if (framesToUpdate > 0) {
        lastFrame += framesToUpdate;
        lastFrame %= frameCount;
        lastUpdate = current;
        int row = lastFrame / sheetDimensions.first;
        int column = lastFrame % sheetDimensions.first;
        rect->x = spriteDimensions.first * column;
        rect->y = spriteDimensions.second * row;
        rect->w = spriteDimensions.first;
        rect->h = spriteDimensions.second;
    }

}