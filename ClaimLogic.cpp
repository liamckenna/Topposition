#include "ClaimLogic.h"

void RefreshClaimNotifs(Peak* individualPeak) {
    if (individualPeak != NULL) {
        for (int i = 0; i < individualPeak->occupants.size(); i++) {
            if (individualPeak->occupants[i]->GetPlayer() == currentTurn && individualPeak->GetClaimedBy() != currentTurn) {
                individualPeak->GetClaimNotif()->SetRendered(true);
                break;
            }
            else {
                individualPeak->GetClaimNotif()->SetRendered(false);
            }
        }
    } else {
        for (int i = 0; i < peaks.size(); i++) {
            for (int j = 0; j < peaks[i]->occupants.size(); j++) {
                if (peaks[i]->occupants[j]->GetPlayer() == currentTurn && peaks[i]->GetClaimedBy() != currentTurn) {
                    peaks[i]->GetClaimNotif()->SetRendered(true);
                    break;
                } else {
                    peaks[i]->GetClaimNotif()->SetRendered(false);
                }
            }
            if (peaks[i]->occupants.size() == 0) peaks[i]->GetClaimNotif()->SetRendered(false);
        }
    }
}


void ClaimPeak(UIElement* peakNotif) {
    Peak* peak = peakNotif->GetAssociatedPeak();

    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetPlayer() != currentTurn) {
            PeakBattle(peak, currentTurn, peak->occupants[i]->GetPlayer());
        }
    }
    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetPlayer() == currentTurn) {
            currentTurn->peaks.push_back(peak);
            std::cout << currentTurn->peaks.size() << std::endl;
            std::cout << players[0]->peaks.size() << std::endl;

            peak->Claim(currentTurn);
            if (peak->GetItem() != nullptr) {
                peak->GetItem()->SetOwner(currentTurn);
                peak->GetItem()->SetResizable(false);
                peak->GetItem()->SetSelectable(true);
                currentTurn->inventory.push_back(peak->GetItem());

                peak->SetItem(nullptr);
                for (int j = 0; j < currentTurn->inventory.size(); j++) {
                    currentTurn->inventory[j]->SetScale(0.1);
                    currentTurn->inventory[j]->SetPosition(100 + 70*j, 10);
                }
            }
            for (int j = 0; j < peak->flags.size(); j++) {
                if (peak->flags[j]->GetPlayer() == currentTurn) {
                    peak->flags[j]->SetRendered(true);
                } else {
                    peak->flags[j]->SetRendered(false);
                }
            }
            break;
        }
    }

    UpdateScore();

}

void PeakBattle(Peak* peak, Player* attacker, Player* defender) {
    int attackers = 0;
    int attackPower = 0;
    int defenders = 0;
    int defensePower = 0;
    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetPlayer() == attacker) {
            attackers++;
        } else if (peak->occupants[i]->GetPlayer() == defender) {
            defenders++;
        }
    }
    for (int i = 0; i < attackers; i++) {
        attackPower += rand() % 6 + 1;
    }
    for (int i = 0; i < defenders; i++) {
        defensePower += rand() % 6 + 1;
    }
    std::cout << "Attack power: " << attackPower << std::endl;
    std::cout << "Defense power: " << defensePower << std::endl;
    if (attackPower >= defensePower) {
        Retreat(peak, defender);
    } else {
        Retreat(peak, attacker);
    }
}

void Retreat(Peak* peak, Player* player) {
    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetPlayer() == player) {

            int x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
            int y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
            while (selectTerrain(x, y) != NULL) {
                x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
                y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
            }
            peak->occupants[i]->SetCenter(x, y);
            peak->occupants[i]->SetCurrentAnimation(peak->occupants[i]->animations["floatIdle"]);
            for (int j = 0; j < peak->occupants[i]->GetPlayer()->peaks.size(); j++) {
                if (peak->occupants[i]->GetPlayer()->peaks[j] == peak) {
                    peak->occupants[i]->GetPlayer()->peaks.erase(peak->occupants[i]->GetPlayer()->peaks.begin() + j);
                }
            }
            peak->occupants.erase(peak->occupants.begin() + i);
        }
    }

}