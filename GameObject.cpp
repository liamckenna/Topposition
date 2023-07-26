#include "GameObject.h"
#include <iostream>

GameObject::GameObject(string name, GPU_Image *texture, SDL_Surface* surface, bool m, bool r) {
    this->name = name;
    this->texture = texture;
    this->surface = surface;
    size = 1;
    dimensions.first = texture->w;
    dimensions.second = texture->h;

    SetPosition(0,0, true);
    SetCenter();
    SetDefaultPosition(0, 0);
    movable = m;
    rendered = r;
}

void GameObject::RenderGameObject(GPU_Target* window) {
    if (size == 1) {
        renderRect->x = defaultPosition.first;
        renderRect->y = defaultPosition.second;
    } else {
        renderRect->x = position.first;
        renderRect->y = position.second;
    }
    dimensions.first = texture->w;
    dimensions.second = texture->h;
    //std::cout << dimensions.first << ", " << dimensions.second << std::endl;

    renderRect->w = (float) (dimensions.first * size * scale);
    renderRect->h = (float) (dimensions.second * size * scale);
    if (rendered) {
        if (currentAnimation == nullptr) {
            GPU_BlitRect(texture, NULL, window, renderRect);
        } else {
            GPU_BlitRect(currentAnimation->GetSpriteSheet(), currentAnimation->GetRect(), window, renderRect);
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

void Terrain::RenderGameObject(GPU_Target* window, Terrain* hoveringTerrain) {
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
        GPU_SetRGBA(pixels, 255/2, 255/2, 255/2, 255);
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

    renderRect->w = ceil(dimensions.first * size * 2);
    renderRect->h = ceil(dimensions.second * size * 2);
    if (rendered) {
        GPU_BlitRect(pixels, NULL, window, renderRect);
    }


    if (hovering) {
        GPU_SetRGBA(pixels, 255, 255, 255, 255);
    }
}

void Pixel::RenderGameObject(GPU_Target* window, Terrain* hoveringTerrain) {


    GPU_LoadTarget(hiddenTerrain->GetPixels());

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
        GPU_SetRGBA(texture, color.r/2, color.g/2, color.b/2, color.a);
    } else {
        GPU_SetRGBA(texture, color.r, color.g, color.b, color.a);
    }

    if (size == 1) {
        renderRect->x = position.first - hiddenTerrain->GetPosition().first;
        renderRect->y = position.second - hiddenTerrain->GetPosition().second;
    } else {
        renderRect->x = position.first - hiddenTerrain->GetPosition().first;
        renderRect->y = position.second - hiddenTerrain->GetPosition().second;
    }
    renderRect->w = (width + 2);
    renderRect->h = (height + 2);

    GPU_BlitRect(texture, NULL, window, renderRect);
    if (hovering) {
        GPU_SetRGBA(texture, color.r, color.g, color.b, color.a);
    }
}

Text::Text(string n, const char* fp, SDL_Color c, int x, int y, int w, int h, int s, const char* t) {
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
    texture = GPU_CopyImageFromSurface(surface);
}

void Text::RenderText(GPU_Target* window) {
    if (rendered) {
        rect->x = position.first;
        rect->y = position.second;
        rect->w = dimensions.first;
        rect->h = dimensions.second;
        GPU_BlitRect(texture, NULL, window, rect);
    }
}

Animation::Animation(GPU_Image *ss, SDL_Surface *s, float d, int fc, pair<int, int> sd, pair<int, int> spd) {
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