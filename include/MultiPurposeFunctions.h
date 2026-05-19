#pragma once
#include "GlobalVars.h"
#include <iostream>

SDL_Color GetPixelColor(const SDL_Surface* surface, const int X, const int Y);

void Print(string s);

void HueShift(int &r, int &g, int &b, float shiftDegrees);

std::pair<float, float> GetRelativePosition(float globalX, float globalY);

std::pair<float, float> GetGlobalPosition(float relativeX, float relativeY);