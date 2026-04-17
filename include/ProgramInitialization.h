#pragma once
#include "GlobalVars.h"

bool init();

void close();

SDL_Surface* loadSurface( std::string path );

SDL_Texture* loadTexture( std::string path );

void TextureLoader();
