#include "ClaimLogic.h"
#include <chrono>
#include <thread>

void RefreshClaimNotifs()
{
    for (int i = 0; i < peaks.size(); i++)
    {
        for (int j = 0; j < peaks[i]->occupants.size(); j++)
        {
            if (peaks[i]->occupants[j]->GetPlayer() == currentTurn && peaks[i]->GetClaimedBy() != currentTurn)
            {
                peaks[i]->occupants[j]->UpdateRelativePositions();
                peaks[i]->GetClaimNotif()->SetGlobalPosition((peaks[i]->occupants[j]->GetCenter().first / cameraZoom) + cameraPosition.first,
                                                             (peaks[i]->occupants[j]->GetCenter().second / cameraZoom) + cameraPosition.second - 150);
                peaks[i]->GetClaimNotif()->SetRendered(true);
                break;
            }
            else
            {
                peaks[i]->GetClaimNotif()->SetRendered(false);
            }
        }
        if (peaks[i]->occupants.size() == 0)
            peaks[i]->GetClaimNotif()->SetRendered(false);
    }
}

void ClaimPeak(UIElement *peakNotif)
{
    Peak *peak = peakNotif->GetAssociatedPeak();

    while (IsOccupyingPeak(peak, currentTurn) && !LastPlayerStanding(peak, currentTurn))
    {
        for (int i = 0; i < peak->occupants.size(); i++)
        {
            if (peak->occupants[i]->GetPlayer() != currentTurn)
            {
                Player *winner = PeakBattle(peak, currentTurn, peak->occupants[i]->GetPlayer());
                break;
            }
        }
    }
    if (LastPlayerStanding(peak, currentTurn))
    {
        peak->Claim(currentTurn);
        if (peak->GetItem() != nullptr)
        {
            peak->GetItem()->SetOwner(currentTurn);
            peak->GetItem()->SetResizable(false);
            peak->GetItem()->SetTopLayer(false);
            peak->GetItem()->SetSelectable(true);
            currentTurn->inventory.push_back(peak->GetItem());

            peak->SetItem(nullptr);
            for (int j = 0; j < currentTurn->inventory.size(); j++)
            {
                currentTurn->inventory[j]->SetScale(0.1);
                currentTurn->inventory[j]->SetGlobalPosition(100 + 70 * j, 10);
            }
        }
        for (int j = 0; j < peak->flags.size(); j++)
        {
            if (peak->flags[j]->GetPlayer() == currentTurn)
            {
                peak->flags[j]->SetRendered(true);
            }
            else
            {
                peak->flags[j]->SetRendered(false);
            }
        }
    }
    UpdateScore();
}

Player *PeakBattle(Peak *peak, Player *attacker, Player *defender)
{
    std::vector<Piece *> attackers;
    std::vector<Piece *> defenders;

    for (int i = 0; i < peak->occupants.size(); i++)
    {
        if (peak->occupants[i]->GetPlayer() == attacker)
        {
            attackers.push_back(peak->occupants[i]);
        }
        else if (peak->occupants[i]->GetPlayer() == defender)
        {
            defenders.push_back(peak->occupants[i]);
        }
    }
    Player *loser = nullptr;
    Player *winner = nullptr;
    while (attackers.size() > 0 && defenders.size() > 0)
    {
        Piece *losing_piece = PieceBattle(peak, attackers[attackers.size() - 1], defenders[defenders.size() - 1]);
        if (losing_piece->GetPlayer() == attacker)
        {
            attackers.pop_back();
            loser = attacker;
            winner = defender;
        }
        else
        {
            defenders.pop_back();
            loser = defender;
            winner = attacker;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (loser == defender)
    {
        RetreatPlayer(peak, defender);
    }

    return winner;
}

Piece *PieceBattle(Peak *peak, Piece *attacker, Piece *defender)
{
    int attackPower = rand() % 6 + 1;
    int defensePower = rand() % 6 + 1;

    std::cout << std::endl;

    printf("Attack roll: %d vs Defense roll: %d\n", attackPower, defensePower);

    if (attackPower >= defensePower)
    {
        printf("%s wins the battle!\n", attacker->GetPlayer()->GetName().c_str());
        RetreatPiece(peak, defender);
        return defender;
    }
    else
    {
        printf("%s wins the battle!\n", defender->GetPlayer()->GetName().c_str());
        RetreatPiece(peak, attacker);
        return attacker;
    }
}

void RetreatPiece(Peak *peak, Piece *piece)
{
    int x = (rand() % (int)(SCREEN_WIDTH / 1.05)) + SCREEN_WIDTH / 2 - SCREEN_WIDTH / 2.1;
    int y = (rand() % (int)(SCREEN_HEIGHT / 1.05)) + SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 2.1;
    while (selectTerrain(x, y) != NULL)
    {
        x = (rand() % (int)(SCREEN_WIDTH / 1.05)) + SCREEN_WIDTH / 2 - SCREEN_WIDTH / 2.1;
        y = (rand() % (int)(SCREEN_HEIGHT / 1.05)) + SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 2.1;
    }
    piece->SetBottomMiddle(x, y);
    piece->SetCurrentAnimation(piece->animations["floatIdle"]);
    for (int i = 0; i < peak->occupants.size(); i++)
    {
        if (peak->occupants[i] == piece)
        {
            peak->occupants.erase(peak->occupants.begin() + i);
            break;
        }
    }
}

void RetreatPlayer(Peak *peak, Player *player)
{
    for (int i = 0; i < player->peaks.size(); i++)
    {
        if (player->peaks[i] == peak)
        {
            player->peaks.erase(player->peaks.begin() + i);
        }
    }
}

bool LastPlayerStanding(Peak *peak, Player *player)
{
    for (int i = 0; i < peak->occupants.size(); i++)
    {
        if (peak->occupants[i]->GetPlayer() != player)
            return false;
    }
    return true;
}

bool IsOccupyingPeak(Peak *peak, Player *player)
{
    for (int i = 0; i < peak->occupants.size(); i++)
    {
        if (peak->occupants[i]->GetPlayer() == player)
            return true;
    }
    return false;
}