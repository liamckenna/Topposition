#include "ClaimLogic.h"

namespace
{
    enum BattlePhase
    {
        BATTLE_IDLE,
        BATTLE_START_ROUND,
        BATTLE_WAIT_ATTACK_ROLL,
        BATTLE_WAIT_DEFENSE_ROLL,
        BATTLE_RESOLVE_ROUND,
        BATTLE_ADVANCE
    };

    struct BattleSequenceState
    {
        bool active = false;
        Peak *peak = nullptr;
        Player *attacker = nullptr;
        Player *defender = nullptr;
        std::vector<Piece *> attackers;
        std::vector<Piece *> defenders;
        Piece *roundAttacker = nullptr;
        Piece *roundDefender = nullptr;
        int attackRoll = 0;
        int defenseRoll = 0;
        Uint64 phaseStart = 0;
        BattlePhase phase = BATTLE_IDLE;
    };

    BattleSequenceState battleSequence;

    bool HasActiveDiceAnimation()
    {
        for (size_t i = 0; i < diceAnimations.size(); i++)
        {
            if (!diceAnimations[i].finished)
            {
                return true;
            }
        }
        return false;
    }

    UIElement *FindDieElement(const std::string &name)
    {
        for (size_t i = 0; i < uiElements.size(); i++)
        {
            if (uiElements[i]->GetName() == name)
            {
                return uiElements[i];
            }
        }
        return nullptr;
    }

    void StartSingleDieRoll(const std::string &dieName, int roll)
    {
        UIElement *die = FindDieElement(dieName);
        if (die == nullptr)
        {
            return;
        }

        std::vector<SDL_Texture *> dieFaces;
        for (int i = 1; i <= 6; i++)
        {
            dieFaces.push_back(textures["die " + std::to_string(i)][0]);
        }

        std::string finalFaceName = "die " + std::to_string(roll);
        Uint64 now = SDL_GetTicks();
        diceAnimations.clear();
        diceAnimations.emplace_back(die, dieFaces, textures[finalFaceName][0], now, 200, 5, false);
    }

    void RefreshBattleParticipants()
    {
        battleSequence.attackers.clear();
        battleSequence.defenders.clear();

        for (size_t i = 0; i < battleSequence.peak->occupants.size(); i++)
        {
            if (battleSequence.peak->occupants[i]->GetPlayer() == battleSequence.attacker)
            {
                battleSequence.attackers.push_back(battleSequence.peak->occupants[i]);
            }
            else if (battleSequence.peak->occupants[i]->GetPlayer() == battleSequence.defender)
            {
                battleSequence.defenders.push_back(battleSequence.peak->occupants[i]);
            }
        }
    }

    bool BeginBattleAgainstDefender(Peak *peak, Player *attacker)
    {
        for (size_t i = 0; i < peak->occupants.size(); i++)
        {
            if (peak->occupants[i]->GetPlayer() != attacker)
            {
                battleSequence.active = true;
                battleSequence.peak = peak;
                battleSequence.attacker = attacker;
                battleSequence.defender = peak->occupants[i]->GetPlayer();
                battleSequence.roundAttacker = nullptr;
                battleSequence.roundDefender = nullptr;
                battleSequence.attackRoll = 0;
                battleSequence.defenseRoll = 0;
                battleSequence.phaseStart = SDL_GetTicks();
                battleSequence.phase = BATTLE_START_ROUND;
                RefreshBattleParticipants();
                return true;
            }
        }
        return false;
    }

    void EndBattleSequence()
    {
        battleSequence = BattleSequenceState();
    }

    void FinalizePeakClaim(Peak *peak)
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
            for (size_t j = 0; j < currentTurn->inventory.size(); j++)
            {
                currentTurn->inventory[j]->SetScale(0.1);
                currentTurn->inventory[j]->SetGlobalPosition(100 + 70 * j, 10);
            }
        }
        for (size_t j = 0; j < peak->flags.size(); j++)
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
        UpdateScore();
        std::string peaksLeftString = "peaks left: " + to_string(unclaimedPeakCount);
        peaksLeftText->SetTextContent(peaksLeftString.c_str(), renderer);
    }
}

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
    if (IsBattleSequenceActive())
    {
        return;
    }

    Peak *peak = peakNotif->GetAssociatedPeak();
    if (!IsOccupyingPeak(peak, currentTurn))
    {
        return;
    }

    if (LastPlayerStanding(peak, currentTurn))
    {
        FinalizePeakClaim(peak);
        return;
    }

    BeginBattleAgainstDefender(peak, currentTurn);
}

void UpdateBattleSequence()
{
    if (!battleSequence.active)
    {
        return;
    }

    if (battleSequence.peak == nullptr || battleSequence.attacker == nullptr || !IsOccupyingPeak(battleSequence.peak, battleSequence.attacker))
    {
        EndBattleSequence();
        RefreshClaimNotifs();
        return;
    }

    switch (battleSequence.phase)
    {
    case BATTLE_START_ROUND:
        if (battleSequence.attackers.size() < 1 || battleSequence.defenders.size() < 1)
        {
            battleSequence.phase = BATTLE_ADVANCE;
            battleSequence.phaseStart = SDL_GetTicks();
            break;
        }

        battleSequence.roundAttacker = battleSequence.attackers[battleSequence.attackers.size() - 1];
        battleSequence.roundDefender = battleSequence.defenders[battleSequence.defenders.size() - 1];
        battleSequence.attackRoll = rand() % 6 + 1;
        StartSingleDieRoll("dieOne", battleSequence.attackRoll);
        battleSequence.phase = BATTLE_WAIT_ATTACK_ROLL;
        break;
    case BATTLE_WAIT_ATTACK_ROLL:
        if (HasActiveDiceAnimation())
            break;

        battleSequence.defenseRoll = rand() % 6 + 1;
        StartSingleDieRoll("dieTwo", battleSequence.defenseRoll);
        battleSequence.phase = BATTLE_WAIT_DEFENSE_ROLL;
        break;
    case BATTLE_WAIT_DEFENSE_ROLL:
        if (HasActiveDiceAnimation())
            break;

        battleSequence.phase = BATTLE_RESOLVE_ROUND;
        break;
    case BATTLE_RESOLVE_ROUND:
    {
        if (battleSequence.roundAttacker == nullptr || battleSequence.roundDefender == nullptr)
        {
            battleSequence.phase = BATTLE_START_ROUND;
            break;
        }

        std::cout << std::endl;
        printf("Attack roll: %d vs Defense roll: %d\n", battleSequence.attackRoll, battleSequence.defenseRoll);

        if (battleSequence.attackRoll >= battleSequence.defenseRoll)
        {
            printf("%s wins the battle!\n", battleSequence.roundAttacker->GetPlayer()->GetName().c_str());
            RetreatPiece(battleSequence.peak, battleSequence.roundDefender);
            battleSequence.defenders.pop_back();
        }
        else
        {
            printf("%s wins the battle!\n", battleSequence.roundDefender->GetPlayer()->GetName().c_str());
            RetreatPiece(battleSequence.peak, battleSequence.roundAttacker);
            battleSequence.attackers.pop_back();
        }

        battleSequence.roundAttacker = nullptr;
        battleSequence.roundDefender = nullptr;
        battleSequence.phaseStart = SDL_GetTicks();
        battleSequence.phase = BATTLE_ADVANCE;
        break;
    }
    case BATTLE_ADVANCE:
        if (SDL_GetTicks() - battleSequence.phaseStart < 250)
        {
            break;
        }

        if (battleSequence.attackers.size() > 0 && battleSequence.defenders.size() > 0)
        {
            battleSequence.phase = BATTLE_START_ROUND;
            break;
        }

        if (battleSequence.defenders.size() < 1)
        {
            RetreatPlayer(battleSequence.peak, battleSequence.defender);
        }

        if (LastPlayerStanding(battleSequence.peak, battleSequence.attacker))
        {
            FinalizePeakClaim(battleSequence.peak);
            EndBattleSequence();
            RefreshClaimNotifs();
            break;
        }

        if (!BeginBattleAgainstDefender(battleSequence.peak, battleSequence.attacker))
        {
            EndBattleSequence();
            RefreshClaimNotifs();
        }
        break;
    case BATTLE_IDLE:
    default:
        break;
    }
}

bool IsBattleSequenceActive()
{
    return battleSequence.active;
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
