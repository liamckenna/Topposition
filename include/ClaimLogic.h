#pragma once
#include "GlobalVars.h"
#include "GameLogic.h"
#include "MouseLogic.h"

void RefreshClaimAndDefendNotifs();

void ClaimPeak(UIElement *claimNotif);

void DefendPeak(UIElement *defendNotif);

bool HasActiveDiceAnimation();

void RetreatPiece(Peak *peak, Piece *piece);

void RetreatPlayer(Peak *peak, Player *player);

bool LastPlayerStanding(Peak *peak, Player *player);

bool IsOccupyingPeak(Peak *peak, Player *player);

void UpdateBattleSequence();

bool IsBattleSequenceActive();

bool IsBattleWaitingForDieClick(UIElement* die = nullptr);

void OnBattleDieClicked(const std::string &dieName);

void KillSoldier(Piece *soldier);