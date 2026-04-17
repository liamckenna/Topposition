#pragma once
#include "GlobalVars.h"
#include "ClaimLogic.h"

int Roll();

void RotateTurn();

void UpdateMovesLeft();

void FinishTurn();

void UpdateScore();

void GameFinished(Player* winner);

void Tiebreaker();

