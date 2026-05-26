#pragma once
#include "GlobalVars.h"

void RenderScreen();

void renderMenuBackground();

void renderPauseOverlay();

void renderUI();

void renderClaimAndDefendNotifs();

void renderClaimFlags();

void renderTerrain();

void renderPieces();

void renderSelectedObject();

void renderInventory();

void renderText();

void renderOcean();

void renderOceanFrozen();

void CheckOceanTilePositioning();

void UpdateFPSCounter(int fps);

void ReflectOceanTile(OceanTile *tile);

void renderPauseOverlay();

void RenderLoadingScreen();