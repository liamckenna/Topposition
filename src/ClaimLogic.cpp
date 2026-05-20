#include "ClaimLogic.h"
#include "MultiPurposeFunctions.h"

namespace
{
    constexpr int BATTLE_ROUND_DELAY_MS = 1000;
    constexpr int BATTLE_ADVANCE_DELAY_MS = 1000;

    enum BattlePhase
    {
        BATTLE_IDLE,
        BATTLE_START_ROUND,
        BATTLE_WAIT_ATTACK_ROLL,
        BATTLE_REVEAL_ATTACK,
        BATTLE_WAIT_DEFENSE_ROLL,
        BATTLE_RESOLVE_ROUND,
        BATTLE_ADVANCE,
        BATTLE_END
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
        Uint64 phaseStart = SDL_GetTicks();
        BattlePhase phase = BATTLE_IDLE;
    };

    BattleSequenceState battleSequence;

    int RollDie()
    {
        return rand() % 6 + 1;
    }

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
        diceAnimations.emplace_back(die, dieFaces, textures[finalFaceName][0], now, 125, 8, false);
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
                opposingPlayerCircle->SetRendered(true);
                SDL_SetTextureColorMod(opposingPlayerCircle->GetTexture(), battleSequence.defender->GetSDLColor().r / 1.5f, battleSequence.defender->GetSDLColor().g / 1.5f, battleSequence.defender->GetSDLColor().b / 1.5f);
                battleSequence.defender->GetCircleText()->SetTextContent(std::to_string(battleSequence.defenders.size()).c_str(), renderer);
                battleSequence.defender->GetCircleText()->SetCenter(opposingPlayerCircle->GetCenter().first, opposingPlayerCircle->GetCenter().second);
                battleSequence.defender->GetCircleText()->SetRendered(true);
                endTurnArrow->SetRendered(false);
                endText->SetRendered(false);
                turnText->SetRendered(false);
                battleSequence.peak->GetClaimNotif()->SetRendered(false);
                crown->SetRendered(false);
                battleSequence.attacker->GetCircleText()->SetTextContent(std::to_string(battleSequence.attackers.size()).c_str(), renderer);
                battleSequence.attacker->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);

                return true;
            }
        }
        return false;
    }

    void EndBattleSequence()
    {
        battleSequence.attacker->GetCircleText()->SetTextContent(battleSequence.attacker->GetCircleTextString().c_str(), renderer);
        battleSequence.defender->GetCircleText()->SetTextContent(battleSequence.defender->GetCircleTextString().c_str(), renderer);
        battleSequence.attacker->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        battleSequence.defender->GetCircleText()->SetCenter(opposingPlayerCircle->GetCenter().first, opposingPlayerCircle->GetCenter().second);
        battleSequence.defender->GetCircleText()->SetRendered(false);
        battleSequence.defender->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        opposingPlayerCircle->SetRendered(false);
        endTurnArrow->SetRendered(true);
        endText->SetRendered(true);
        turnText->SetRendered(true);
        crown->SetRendered(false);
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
        RefreshClaimNotifs();
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

    if (battleSequence.peak == nullptr || battleSequence.attacker == nullptr || !IsOccupyingPeak(battleSequence.peak, battleSequence.attacker) && battleSequence.phase != BATTLE_END)
    {
        battleSequence.phase = BATTLE_END;
        battleSequence.phaseStart = SDL_GetTicks();
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
        battleSequence.attackRoll = RollDie();
        StartSingleDieRoll("dieOne", battleSequence.attackRoll);
        battleSequence.phase = BATTLE_WAIT_ATTACK_ROLL;
        break;
    case BATTLE_WAIT_ATTACK_ROLL:
        if (HasActiveDiceAnimation())
            break;

        battleSequence.phase = BATTLE_REVEAL_ATTACK;
        battleSequence.phaseStart = SDL_GetTicks();
        break;
    case BATTLE_REVEAL_ATTACK:
        if (SDL_GetTicks() - battleSequence.phaseStart < BATTLE_ADVANCE_DELAY_MS)
        {
            break;
        }
        battleSequence.defenseRoll = RollDie();
        StartSingleDieRoll("dieTwo", battleSequence.defenseRoll);
        battleSequence.phase = BATTLE_WAIT_DEFENSE_ROLL;
        battleSequence.phaseStart = SDL_GetTicks();
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

        if (battleSequence.attackRoll >= battleSequence.defenseRoll)
        {

            crown->SetGlobalPosition(die1->GetGlobalCenter().first - (die1->GetDimensions().first * die1->GetScale() / 2) - (crown->GetDimensions().first * crown->GetScale()), die1->GetGlobalCenter().second - (die1->GetDimensions().second * die1->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
            crown->SetRotation(-45);
            crown->SetRendered(true);
            RetreatPiece(battleSequence.peak, battleSequence.roundDefender);
            if (battleSequence.attackRoll - battleSequence.defenseRoll >= rules->GetFatalBattleDifference())
            {
                battleSequence.roundDefender->SetRendered(false);
                battleSequence.roundDefender->SetMovable(false);
                battleSequence.roundDefender->SetSelectable(false);
                battleSequence.roundDefender->SetCurrentAnimation(nullptr);
                battleSequence.roundDefender->SetDead(true);
            }
            battleSequence.defenders.pop_back();
            battleSequence.defender->GetCircleText()->SetTextContent(std::to_string(battleSequence.defenders.size()).c_str(), renderer);
            battleSequence.defender->GetCircleText()->SetCenter(opposingPlayerCircle->GetCenter().first, opposingPlayerCircle->GetCenter().second);
        }
        else
        {
            crown->SetGlobalPosition(die2->GetGlobalCenter().first + (die2->GetDimensions().first * die2->GetScale() / 2), die2->GetGlobalCenter().second - (die2->GetDimensions().second * die2->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
            crown->SetRotation(45);
            crown->SetRendered(true);
            RetreatPiece(battleSequence.peak, battleSequence.roundAttacker);
            if (battleSequence.defenseRoll - battleSequence.attackRoll >= rules->GetFatalBattleDifference())
            {
                battleSequence.roundAttacker->SetRendered(false);
                battleSequence.roundAttacker->SetMovable(false);
                battleSequence.roundAttacker->SetSelectable(false);
                battleSequence.roundAttacker->SetCurrentAnimation(nullptr);
                battleSequence.roundAttacker->SetDead(true);
            }
            battleSequence.attackers.pop_back();
            battleSequence.attacker->GetCircleText()->SetTextContent(std::to_string(battleSequence.attackers.size()).c_str(), renderer);
            battleSequence.attacker->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        }

        battleSequence.roundAttacker = nullptr;
        battleSequence.roundDefender = nullptr;
        battleSequence.phaseStart = SDL_GetTicks();
        battleSequence.phase = BATTLE_ADVANCE;
        break;
    }
    case BATTLE_ADVANCE:

        if (LastPlayerStanding(battleSequence.peak, battleSequence.attacker))
        {
            FinalizePeakClaim(battleSequence.peak);
            battleSequence.phase = BATTLE_END;
            battleSequence.phaseStart = SDL_GetTicks();
            break;
        }

        if (SDL_GetTicks() - battleSequence.phaseStart < BATTLE_ROUND_DELAY_MS)
        {
            break;
        }
        crown->SetRendered(false);

        if (battleSequence.attackers.size() > 0 && battleSequence.defenders.size() > 0)
        {
            battleSequence.phase = BATTLE_START_ROUND;
            break;
        }

        if (battleSequence.defenders.size() < 1)
        {
            RetreatPlayer(battleSequence.peak, battleSequence.defender);
            battleSequence.defender->GetCircleText()->SetRendered(false);
            battleSequence.defender->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        }

        if (!BeginBattleAgainstDefender(battleSequence.peak, battleSequence.attacker))
        {
            battleSequence.phase = BATTLE_END;
            battleSequence.phaseStart = SDL_GetTicks();
        }
        break;
    case BATTLE_IDLE:
        break;
    case BATTLE_END:

        if (SDL_GetTicks() - battleSequence.phaseStart < BATTLE_ROUND_DELAY_MS)
        {
            break;
        }
        EndBattleSequence();
        RefreshClaimNotifs();
        break;
    default:
        break;
    }
}

bool IsBattleSequenceActive()
{
    return battleSequence.active;
}

void RetreatPiece(Peak *peak, Piece *piece)
{
    bool foundRetreatLocation = false;
    int retreatDistance = 100;
    while (!foundRetreatLocation)
    {
        float xDir = (rand() % 200 - 100) / 100.0f;
        float yDir = (rand() % 200 - 100) / 100.0f;

        float length = sqrt(xDir * xDir + yDir * yDir);
        if (length == 0)
        {
            xDir = 0;
            yDir = -1;
        }
        else
        {
            xDir /= length;
            yDir /= length;
        }

        float globalX = piece->GetGlobalCenter().first + xDir * retreatDistance;
        float globalY = piece->GetGlobalCenter().second + yDir * retreatDistance;
        std::pair<float, float> relativePos = GetRelativePosition(globalX, globalY);
        int x = relativePos.first;
        int y = relativePos.second;
        if (selectTerrain(x, y) == nullptr)
        {
            foundRetreatLocation = true;
            piece->SetBottomMiddle(x, y);
            piece->SetCurrentAnimation(piece->animations["floatIdle"]);
        }
        else {
            retreatDistance += 50;
        }
    }
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

int SoldierCount(Player *player)
{
    int count = 0;
    for (size_t i = 0; i < player->soldiers.size(); i++)
    {
        if (!player->soldiers[i]->IsDead())
        {
            count++;
        }
    }
    return count;
}