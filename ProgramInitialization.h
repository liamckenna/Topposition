#pragma once
#include "GlobalVars.h"
#include "SDL2/SDL_gpu.h"

bool init();

void close();

SDL_Surface* loadSurface( std::string path );

GPU_Image* loadTexture( std::string path );

void TextureLoader();
