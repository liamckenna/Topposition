#include "MultiPurposeFunctions.h"

SDL_Color GetPixelColor(const SDL_Surface *surface, const int X, const int Y)
{
    SDL_Color Color = {0, 0, 0, 0};
    if (!surface || !surface->pixels)
        return Color;
    if (X < 0 || Y < 0 || X >= surface->w || Y >= surface->h)
        return Color;

    const Uint8 Bpp = SDL_BYTESPERPIXEL(surface->format);
    Uint8 *pPixel = (Uint8 *)surface->pixels + Y * surface->pitch + X * Bpp;

    Uint32 PixelData = 0;
    memcpy(&PixelData, pPixel, Bpp);

    SDL_GetRGBA(PixelData, SDL_GetPixelFormatDetails(surface->format), NULL, &Color.r, &Color.g, &Color.b, &Color.a);
    return Color;
}

void Print(string s)
{
    // std::cout << s << std::endl;
}
