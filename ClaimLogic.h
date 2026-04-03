#pragma once
#include "GlobalVars.h"
#include "GameLogic.h"
#include "MouseLogic.h"

void RefreshClaimNotifs();

void ClaimPeak(UIElement *claimNotif);

Player *PeakBattle(Peak *peak, Player *attacker, Player *defender);

Piece *PieceBattle(Peak *peak, Piece *attacker, Piece *defender);

void RetreatPiece(Peak *peak, Piece *piece);

void RetreatPlayer(Peak *peak, Player *player);

bool LastPlayerStanding(Peak *peak, Player *player);

bool IsOccupyingPeak(Peak *peak, Player *player);