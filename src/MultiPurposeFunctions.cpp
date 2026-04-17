#include "MultiPurposeFunctions.h"
#include <algorithm>
#include <cmath>

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

void HueShift(int& r, int& g, int& b, float shiftDegrees)
{
    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;

    float maxc = std::max({rf, gf, bf});
    float minc = std::min({rf, gf, bf});
    float delta = maxc - minc;

    float h = 0.0f;
    float s = (maxc == 0.0f) ? 0.0f : (delta / maxc);
    float v = maxc;

    if (delta != 0.0f)
    {
        if (maxc == rf)
            h = 60.0f * std::fmod(((gf - bf) / delta), 6.0f);
        else if (maxc == gf)
            h = 60.0f * (((bf - rf) / delta) + 2.0f);
        else
            h = 60.0f * (((rf - gf) / delta) + 4.0f);

        if (h < 0.0f)
            h += 360.0f;
    }

    float newH = std::fmod(h + shiftDegrees + 360.0f, 360.0f);

    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(newH / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r1 = 0, g1 = 0, b1 = 0;

    if (newH < 60)
    {
        r1 = c;
        g1 = x;
        b1 = 0;
    }
    else if (newH < 120)
    {
        r1 = x;
        g1 = c;
        b1 = 0;
    }
    else if (newH < 180)
    {
        r1 = 0;
        g1 = c;
        b1 = x;
    }
    else if (newH < 240)
    {
        r1 = 0;
        g1 = x;
        b1 = c;
    }
    else if (newH < 300)
    {
        r1 = x;
        g1 = 0;
        b1 = c;
    }
    else
    {
        r1 = c;
        g1 = 0;
        b1 = x;
    }

    r = static_cast<int>((r1 + m) * 255.0f + 0.5f);
    g = static_cast<int>((g1 + m) * 255.0f + 0.5f);
    b = static_cast<int>((b1 + m) * 255.0f + 0.5f);

}