#pragma once
#include "GlobalVars.h"
#include "GameLogic.h"
#include "MouseLogic.h"

void RefreshClaimNotifs();

void ClaimPeak(UIElement *claimNotif);

void RetreatPiece(Peak *peak, Piece *piece);

void RetreatPlayer(Peak *peak, Player *player);

bool LastPlayerStanding(Peak *peak, Player *player);

bool IsOccupyingPeak(Peak *peak, Player *player);

void UpdateBattleSequence();

bool IsBattleSequenceActive();

void KillSoldier(Piece *soldier);