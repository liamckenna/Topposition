#pragma once
#include "GlobalVars.h"
#include "GameLogic.h"
#include "MouseLogic.h"

void RefreshClaimNotifs(Peak* individualPeak = NULL);

void ClaimPeak(UIElement* claimNotif);

void PeakBattle(Peak* peak, Player* attacker, Player* defender);

void Retreat(Peak* peak, Player* player);
