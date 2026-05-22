#include "ClaimLogic.h"
#include "MultiPurposeFunctions.h"
#include "AudioManager.h"
#include <algorithm>

namespace
{
    constexpr int BATTLE_ROUND_DELAY_MS = 1000;
    constexpr int BATTLE_ADVANCE_DELAY_MS = 0;
    constexpr int BATTLE_FATAL_SIGNAL_DELAY_MS = 1800;

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
        else if (die == die1)
        {
            AudioManager::playSound("roll-left");
            AudioManager::playSound("grunt-left");
        }
        else if (die == die2)
        {
            AudioManager::playSound("roll-right");
            AudioManager::playSound("grunt-right");
        }

        std::vector<SDL_Texture *> dieFaces;
        for (int i = 1; i <= 6; i++)
        {
            dieFaces.push_back(textures["die " + std::to_string(i)][0]);
        }

        std::string finalFaceName = "die " + std::to_string(roll);
        Uint64 now = SDL_GetTicks();
        if (rules->GetAutoRollMoves())
        {
            diceAnimations.erase(std::remove_if(diceAnimations.begin(), diceAnimations.end(),
                                                [die](const DiceAnimation &a)
                                                { return a.die == die; }),
                                 diceAnimations.end());
        }
        else
        {
            diceAnimations.clear();
        }
        diceAnimations.emplace_back(die, dieFaces, textures[finalFaceName][0], now, 125, 8, false);
    }

    void RefreshBattleParticipants()
    {
        battleSequence->attackers.clear();
        battleSequence->defenders.clear();

        for (size_t i = 0; i < battleSequence->peak->occupants.size(); i++)
        {
            if (battleSequence->peak->occupants[i]->GetPlayer() == battleSequence->attacker)
            {
                battleSequence->attackers.push_back(battleSequence->peak->occupants[i]);
            }
            else if (battleSequence->peak->occupants[i]->GetPlayer() == battleSequence->defender)
            {
                battleSequence->defenders.push_back(battleSequence->peak->occupants[i]);
            }
        }
    }

    bool BeginBattleAgainstDefender(Peak *peak, Player *attacker)
    {
        for (size_t i = 0; i < peak->occupants.size(); i++)
        {
            if (peak->occupants[i]->GetPlayer() != attacker)
            {
                battleSequence->active = true;
                battleSequence->peak = peak;
                battleSequence->attacker = attacker;
                battleSequence->defender = peak->occupants[i]->GetPlayer();
                battleSequence->roundAttacker = nullptr;
                battleSequence->roundDefender = nullptr;
                battleSequence->attackRoll = 0;
                battleSequence->defenseRoll = 0;
                battleSequence->phaseStart = SDL_GetTicks();
                battleSequence->phase = BATTLE_START_ROUND;
                RefreshBattleParticipants();
                opposingPlayerCircle->SetRendered(true);
                SDL_SetTextureColorMod(opposingPlayerCircle->GetTexture(), battleSequence->defender->GetSDLColor().r / 1.5f, battleSequence->defender->GetSDLColor().g / 1.5f, battleSequence->defender->GetSDLColor().b / 1.5f);
                battleSequence->defender->GetCircleText()->SetTextContent(std::to_string(battleSequence->defenders.size()).c_str(), renderer);
                battleSequence->defender->GetCircleText()->SetCenter(opposingPlayerCircle->GetCenter().first, opposingPlayerCircle->GetCenter().second);
                battleSequence->defender->GetCircleText()->SetRendered(true);
                endTurnArrow->SetRendered(false);
                endText->SetRendered(false);
                turnText->SetRendered(false);
                battleSequence->peak->GetClaimNotif()->SetRendered(false);
                crown->SetRendered(false);
                battleSequence->attacker->GetCircleText()->SetTextContent(std::to_string(battleSequence->attackers.size()).c_str(), renderer);
                battleSequence->attacker->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
                if (rules->GetClaimEndsTurn())
                {
                    ClearRoll();
                }
                else
                {
                    string rollText = "Roll!";
                    std::pair<float, float> center = movesLeftText->GetCenter();
                    movesLeftText->SetSize(100 * (SCREEN_WIDTH / 3840.f), renderer);
                    movesLeftText->SetTextContent(rollText.c_str(), renderer);
                    movesLeftText->SetCenter(center.first, center.second);
                    //movesLeftText->SetRendered(false);
                }
                RefreshClaimNotifs();
                return true;
            }
        }
        RefreshClaimNotifs();
        return false;
    }

    void EndBattleSequence()
    {
        battleSequence->attacker->GetCircleText()->SetTextContent(battleSequence->attacker->GetCircleTextString().c_str(), renderer);
        battleSequence->defender->GetCircleText()->SetTextContent(battleSequence->defender->GetCircleTextString().c_str(), renderer);
        battleSequence->attacker->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        battleSequence->defender->GetCircleText()->SetCenter(opposingPlayerCircle->GetCenter().first, opposingPlayerCircle->GetCenter().second);
        battleSequence->defender->GetCircleText()->SetRendered(false);
        battleSequence->defender->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        opposingPlayerCircle->SetRendered(false);
        endTurnArrow->SetRendered(true);
        endText->SetRendered(true);
        turnText->SetRendered(true);
        crown->SetRendered(false);

        //movesLeftText->SetRendered(true);
        if (!rules->GetClaimEndsTurn())
        {
            UpdateMovesLeft();
        }

            delete battleSequence;
        battleSequence = new BattleSequenceState();
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
        std::pair<float, float> center = peaksLeftNumText->GetCenter();
        peaksLeftNumText->SetTextContent(to_string(unclaimedPeakCount).c_str(), renderer);
        peaksLeftNumText->SetCenter(center.first, center.second);
    }
}

void RefreshClaimNotifs()
{
    for (int i = 0; i < peaks.size(); i++)
    {
        for (int j = 0; j < peaks[i]->occupants.size(); j++)
        {
            if (peaks[i]->occupants[j]->GetPlayer() == currentTurn && peaks[i]->GetClaimedBy() != currentTurn && !peaks[i]->occupants[j]->IsHeld() && !IsBattleSequenceActive())
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
        if (rules->GetClaimEndsTurn())
            FinishTurn();
        return;
    }

    if (LastPlayerStanding(peak, currentTurn))
    {
        FinalizePeakClaim(peak);
        if (rules->GetClaimEndsTurn())
            FinishTurn();
        return;
    }

    BeginBattleAgainstDefender(peak, currentTurn);
}

void UpdateBattleSequence()
{
    if (!battleSequence->active)
    {
        return;
    }

    switch (battleSequence->phase)
    {
    case BATTLE_START_ROUND:
        if (battleSequence->attackers.size() < 1 || battleSequence->defenders.size() < 1)
        {
            battleSequence->phase = BATTLE_ADVANCE;
            battleSequence->phaseStart = SDL_GetTicks();
            break;
        }

        battleSequence->roundAttacker = battleSequence->attackers[battleSequence->attackers.size() - 1];
        battleSequence->roundDefender = battleSequence->defenders[battleSequence->defenders.size() - 1];
        if (rules->GetAutoRollMoves())
        {
            battleSequence->attackRoll = RollDie();
            StartSingleDieRoll("dieOne", battleSequence->attackRoll);
            battleSequence->phase = BATTLE_WAIT_ATTACK_ROLL;
            break;
        }
        else
        {
            battleSequence->attackRollStarted = false;
            battleSequence->defenseRollStarted = false;
            die1->SetTexture(textures["die 0"][0]);
            die2->SetTexture(textures["die 0"][0]);
            diceAnimations.clear();
            battleSequence->phase = BATTLE_WAIT_DIE_CLICKS;
            break;
        }
    case BATTLE_WAIT_DIE_CLICKS:
        if (battleSequence->attackRollStarted && battleSequence->defenseRollStarted && !HasActiveDiceAnimation())
        {
            battleSequence->phase = BATTLE_RESOLVE_ROUND;
            AudioManager::playSound("crash");
        }
        break;
    case BATTLE_WAIT_ATTACK_ROLL:
        if (HasActiveDiceAnimation())
            break;

        battleSequence->phase = BATTLE_REVEAL_ATTACK;
        battleSequence->phaseStart = SDL_GetTicks();
        break;
    case BATTLE_REVEAL_ATTACK:
        if (SDL_GetTicks() - battleSequence->phaseStart < BATTLE_ADVANCE_DELAY_MS)
        {
            break;
        }
        battleSequence->defenseRoll = RollDie();
        StartSingleDieRoll("dieTwo", battleSequence->defenseRoll);
        battleSequence->phase = BATTLE_WAIT_DEFENSE_ROLL;
        battleSequence->phaseStart = SDL_GetTicks();
        break;
    case BATTLE_WAIT_DEFENSE_ROLL:
        if (HasActiveDiceAnimation())
            break;

        battleSequence->phase = BATTLE_RESOLVE_ROUND;
        AudioManager::playSound("crash");
        break;
    case BATTLE_RESOLVE_ROUND:
    {
        if (battleSequence->roundAttacker == nullptr || battleSequence->roundDefender == nullptr)
        {
            battleSequence->phase = BATTLE_START_ROUND;
            break;
        }

        if (battleSequence->attackRoll >= battleSequence->defenseRoll)
        {

            crown->SetGlobalPosition(die1->GetGlobalCenter().first - (die1->GetDimensions().first * die1->GetScale() / 2) - (crown->GetDimensions().first * crown->GetScale()), die1->GetGlobalCenter().second - (die1->GetDimensions().second * die1->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
            crown->SetRotation(-45);
            crown->SetRendered(true);
            RetreatPiece(battleSequence->peak, battleSequence->roundDefender);
            battleSequence->defenders.pop_back();
            battleSequence->defender->GetCircleText()->SetTextContent(std::to_string(battleSequence->defenders.size()).c_str(), renderer);
            battleSequence->defender->GetCircleText()->SetCenter(opposingPlayerCircle->GetCenter().first, opposingPlayerCircle->GetCenter().second);
        }
        else
        {
            crown->SetGlobalPosition(die2->GetGlobalCenter().first + (die2->GetDimensions().first * die2->GetScale() / 2), die2->GetGlobalCenter().second - (die2->GetDimensions().second * die2->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
            crown->SetRotation(45);
            crown->SetRendered(true);
            RetreatPiece(battleSequence->peak, battleSequence->roundAttacker);
            battleSequence->attackers.pop_back();
            battleSequence->attacker->GetCircleText()->SetTextContent(std::to_string(battleSequence->attackers.size()).c_str(), renderer);
            battleSequence->attacker->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        }

        if (battleSequence->attackRoll - battleSequence->defenseRoll >= rules->GetFatalBattleDifference())
        {
            KillSoldier(battleSequence->roundDefender);
            battleSequence->roundAttacker = nullptr;
            battleSequence->roundDefender = nullptr;
            battleSequence->phaseStart = SDL_GetTicks();
            battleSequence->phase = BATTLE_FATAL_ATTACK;
            break;
        }
        else if (battleSequence->defenseRoll - battleSequence->attackRoll >= rules->GetFatalBattleDifference())
        {
            KillSoldier(battleSequence->roundAttacker);
            battleSequence->roundAttacker = nullptr;
            battleSequence->roundDefender = nullptr;
            battleSequence->phaseStart = SDL_GetTicks();
            battleSequence->phase = BATTLE_FATAL_DEFENSE;
            break;
        }
        else
        {
            AudioManager::playSound("splash", 0.25f);
            battleSequence->roundAttacker = nullptr;
            battleSequence->roundDefender = nullptr;
            battleSequence->phaseStart = SDL_GetTicks();
            battleSequence->phase = BATTLE_ADVANCE;
            break;
        }
        break;
    }
    case BATTLE_FATAL_ATTACK:
    {
        if (SDL_GetTicks() - battleSequence->phaseStart > BATTLE_FATAL_SIGNAL_DELAY_MS)
        {
            fatalAttackExclamation->SetRendered(false);
            battleSequence->phaseStart = SDL_GetTicks();
            battleSequence->phase = BATTLE_FATAL_ADVANCE;
            break;
        }

        switch ((int(SDL_GetTicks() - battleSequence->phaseStart) / (BATTLE_FATAL_SIGNAL_DELAY_MS / 6)) % 2)
        {
            case 0:
                fatalAttackExclamation->SetRendered(true);
                break;
            case 1:
                fatalAttackExclamation->SetRendered(false);
                break;
        }
        break;
    } 
    case BATTLE_FATAL_DEFENSE:
    {
        if (SDL_GetTicks() - battleSequence->phaseStart > BATTLE_FATAL_SIGNAL_DELAY_MS)
        {
            fatalDefenseExclamation->SetRendered(false);
            battleSequence->phaseStart = SDL_GetTicks();
            battleSequence->phase = BATTLE_FATAL_ADVANCE;
            break;
        }

        switch ((int(SDL_GetTicks() - battleSequence->phaseStart) / (BATTLE_FATAL_SIGNAL_DELAY_MS / 6)) % 2)
        {
            case 0:
                fatalDefenseExclamation->SetRendered(true);
                break;
            case 1:
                fatalDefenseExclamation->SetRendered(false);
                break;
        }
        break;
    }
    case BATTLE_ADVANCE:
        if (LastPlayerStanding(battleSequence->peak, battleSequence->attacker))
        {
            FinalizePeakClaim(battleSequence->peak);
            battleSequence->phase = BATTLE_END;
            battleSequence->phaseStart = SDL_GetTicks();
            break;
        }

        if (battleSequence->peak == nullptr || battleSequence->attacker == nullptr || !IsOccupyingPeak(battleSequence->peak, battleSequence->attacker) && battleSequence->phase != BATTLE_END)
        {
            battleSequence->phase = BATTLE_END;
            battleSequence->phaseStart = SDL_GetTicks();
            return;
        }

        if (SDL_GetTicks() - battleSequence->phaseStart < BATTLE_ROUND_DELAY_MS)
        {
            break;
        }
        crown->SetRendered(false);

        if (battleSequence->attackers.size() > 0 && battleSequence->defenders.size() > 0)
        {
            battleSequence->phase = BATTLE_START_ROUND;
            break;
        }

        if (battleSequence->defenders.size() < 1)
        {
            RetreatPlayer(battleSequence->peak, battleSequence->defender);
            battleSequence->defender->GetCircleText()->SetRendered(false);
            battleSequence->defender->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        }

        if (!BeginBattleAgainstDefender(battleSequence->peak, battleSequence->attacker))
        {
            battleSequence->phase = BATTLE_END;
            battleSequence->phaseStart = SDL_GetTicks();
        }
        break;
    case BATTLE_FATAL_ADVANCE:
        if (LastPlayerStanding(battleSequence->peak, battleSequence->attacker))
        {
            FinalizePeakClaim(battleSequence->peak);
            battleSequence->phase = BATTLE_END;
            battleSequence->phaseStart = SDL_GetTicks() - BATTLE_ROUND_DELAY_MS;
            break;
        }

        if (battleSequence->peak == nullptr || battleSequence->attacker == nullptr || !IsOccupyingPeak(battleSequence->peak, battleSequence->attacker) && battleSequence->phase != BATTLE_END)
        {
            battleSequence->phase = BATTLE_END;
            battleSequence->phaseStart = SDL_GetTicks() - BATTLE_ROUND_DELAY_MS;
            return;
        }

        if (battleSequence->attackers.size() > 0 && battleSequence->defenders.size() > 0)
        {
            crown->SetRendered(false);
            battleSequence->phase = BATTLE_START_ROUND;
            break;
        }

        if (SDL_GetTicks() - battleSequence->phaseStart < BATTLE_ROUND_DELAY_MS)
        {
            break;
        }
        crown->SetRendered(false);

        if (battleSequence->defenders.size() < 1)
        {
            RetreatPlayer(battleSequence->peak, battleSequence->defender);
            battleSequence->defender->GetCircleText()->SetRendered(false);
            battleSequence->defender->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        }

        if (!BeginBattleAgainstDefender(battleSequence->peak, battleSequence->attacker))
        {
            battleSequence->phase = BATTLE_END;
            battleSequence->phaseStart = SDL_GetTicks() - BATTLE_ROUND_DELAY_MS;
        }
        break;
        case BATTLE_IDLE:
        break;
    case BATTLE_END:
        if (SDL_GetTicks() - battleSequence->phaseStart < BATTLE_ROUND_DELAY_MS)
        {
            break;
        }
        EndBattleSequence();
        RefreshClaimNotifs();
        if (rules->GetClaimEndsTurn())
            FinishTurn();
        break;
    default:
        break;
    }
}

bool IsBattleSequenceActive()
{
    return battleSequence->active;
}

bool IsBattleWaitingForDieClick(UIElement* die)
{
    if (die == nullptr)
    {
        return battleSequence->active && battleSequence->phase == BATTLE_WAIT_DIE_CLICKS;
    }
    else if (die == die1)
    {
        return battleSequence->active && battleSequence->phase == BATTLE_WAIT_DIE_CLICKS && !battleSequence->attackRollStarted;
    }
    else if (die == die2)
    {
        return battleSequence->active && battleSequence->phase == BATTLE_WAIT_DIE_CLICKS && !battleSequence->defenseRollStarted;
    }
    return false;
}

void OnBattleDieClicked(const std::string &dieName)
{
    if (battleSequence->phase != BATTLE_WAIT_DIE_CLICKS)
        return;

    if (dieName == "dieOne" && !battleSequence->attackRollStarted)
    {
        battleSequence->attackRoll = RollDie();
        StartSingleDieRoll("dieOne", battleSequence->attackRoll);
        battleSequence->attackRollStarted = true;
    }
    else if (dieName == "dieTwo" && !battleSequence->defenseRollStarted)
    {
        battleSequence->defenseRoll = RollDie();
        StartSingleDieRoll("dieTwo", battleSequence->defenseRoll);
        battleSequence->defenseRollStarted = true;
    }
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

void KillSoldier(Piece *soldier)
{
    soldier->SetRendered(false);
    soldier->SetMovable(false);
    soldier->SetSelectable(false);
    soldier->SetCurrentAnimation(nullptr);
    soldier->SetDead(true);
    if (soldier->GetPlayer() == currentTurn)
    {
        soldier->GetPlayer()->soldierHeadCrosses[soldier->GetPlayer()->GetSoldierIndex(soldier)]->SetRendered(true);
    }
    AudioManager::playSound("death");
}