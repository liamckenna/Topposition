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
    renderRect->w = (float) (dimensions.first * size * scale);
    renderRect->h = (float) (dimensions.second * size * scale);
    if (rendered) {
        SDL_RenderCopy(renderer, texture, NULL, renderRect);
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

void GameObject::CycleAnimation(int frame) {
    currentFrame = frame;
    SetTexture(animations[currentAnimation][currentFrame].first);
    std::cout << currentAnimation << ", " << currentFrame << std::endl;
    SetSurface(animations[currentAnimation][currentFrame].second);
}

void Terrain::RenderGameObject(SDL_Renderer *renderer, Terrain* hoveringTerrain) {
    if (this == hoveringTerrain) {
        SDL_SetTextureColorMod(texture, 255, 0, 0);
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
    if (rendered) {
        SDL_RenderCopyEx( renderer, texture, NULL, renderRect, 0, NULL, SDL_FLIP_NONE);
    }
    if (this == hoveringTerrain) {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }
}

void Pixel::RenderGameObject(SDL_Renderer *renderer, Terrain* hoveringTerrain) {
    bool hovering = false;
    if (hoveringTerrain != nullptr) {
        if (hiddenTerrain == hoveringTerrain) {
            hovering = true;
        } else if (hiddenTerrain != nullptr) {
            for (int i = 0; i < hiddenTerrain->connectedTerrain.size(); i++) {
                if (hiddenTerrain->connectedTerrain[i] == hoveringTerrain) {
                    hovering = true;
                    break;
                }
            }
        }
    }

    if (size == 1) {
        renderRect->x = defaultPosition.first;
        renderRect->y = defaultPosition.second;
    } else {
        renderRect->x = position.first;
        renderRect->y = position.second;
    }
    renderRect->w = (width * size * scale);
    renderRect->h = (height * size * scale);
    if (hovering) {
        SDL_SetTextureColorMod(texture, color.r/2, color.g/2, color.b/2);
    } else {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }

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
