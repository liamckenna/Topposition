#include "MultiPurposeFunctions.h"

SDL_Color GetPixelColor(const SDL_Surface * surface, const int X, const int Y)
{
    // Bytes per pixel
    const Uint8 Bpp = surface->format->BytesPerPixel;

    /*
    Retrieve the address to a specific pixel
    pSurface->pixels	= an array containing the SDL_Surface' pixels
    pSurface->pitch		= the length of a row of pixels (in bytes)
    X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
    */
    Uint8* pPixel = (Uint8*)surface->pixels + Y * surface->pitch + X * Bpp;

    Uint32 PixelData = *(Uint32*)pPixel;

    SDL_Color Color = {0x00, 0x00, 0x00};

    // Retrieve the RGB values of the specific pixel
    SDL_GetRGBA(PixelData, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);


    return Color;
}
