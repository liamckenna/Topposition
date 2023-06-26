#include "GameObject.h"
#include <iostream>
void GameObject::RenderGameObject(SDL_Renderer* renderer) {
    if (size == 1) {
        renderRect->x = defaultPosition.first;
        renderRect->y = defaultPosition.second;
    } else {
        renderRect->x = position.first;
        renderRect->y = position.second;
    }
    renderRect->w = dimensions.first * size;
    renderRect->h = dimensions.second * size;
    SDL_RenderCopy( renderer, texture, NULL, renderRect);
}

void GameObject::SetPosition(float x, float y, bool posOnly) {
    position.first = x;
    position.second = y;
    if (!posOnly) {
        SetCenter();
    }
    if (size == 1) {
        defaultPosition.first = position.first;
        defaultPosition.second = position.second;
    }
}

GameObject::GameObject(string name, SDL_Texture *texture, SDL_Surface* surface, bool m, bool r) {
    this->name = name;
    this->texture = texture;
    this->surface = surface;
    size = 1;
    SDL_QueryTexture(texture, NULL, NULL, &dimensions.first, &dimensions.second);
    SetPosition(0,0, true);
    SetCenter();
    originalRatio = dimensions.first/dimensions.second;
    SetDefaultPosition(0, 0);
    movable = m;
    rendered = r;
}

void GameObject::AdjustSize(float multiplier, int w, int h) {

    if (w == 0 || h == 0) {
        size += multiplier;
    } else {
        dimensions.first = w;
        dimensions.second = h;
    }
    SetPosition(center.first - (dimensions.first * size) / 2,center.second - (dimensions.second * size) / 2, true);
}
void GameObject::SetCenter(float x, float y, bool centerOnly){
    if (x == 0 && y == 0) {
        center.first = position.first + (dimensions.first * size)/2;
        center.second = position.second + (dimensions.second * size)/2;
    }
    else {
        if (!centerOnly) {
            center.first = x;
            center.second = y;
            position.first = center.first - (dimensions.first * size) / 2;
            position.second = center.second - (dimensions.second * size) / 2;
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

void Terrain::RenderGameObject(SDL_Renderer *renderer) {
    if (size == 1) {
        renderRect->x = defaultPosition.first;
        renderRect->y = defaultPosition.second;
    } else {
        renderRect->x = position.first;
        renderRect->y = position.second;
    }
    renderRect->w = (dimensions.second * size * scale);
    renderRect->h = (dimensions.second * size * scale);
    SDL_RenderCopy( renderer, texture, NULL, renderRect);
}

void Terrain::SetCenter(float x, float y, bool centerOnly){
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

void Terrain::SetBottomRight() {
    bottomRight.first = position.first + (dimensions.first * size * scale);
    bottomRight.second = position.second + (dimensions.second * size * scale);
}
