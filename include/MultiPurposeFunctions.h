#pragma once
#include "GlobalVars.h"
#include <iostream>

SDL_Color GetPixelColor(const SDL_Surface* surface, const int X, const int Y);

void Print(string s);

void HueShift(int &r, int &g, int &b, float shiftDegrees);