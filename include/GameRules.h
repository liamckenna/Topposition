#pragma once
#include <string>
#include "GameObject.h"
class GameRules
{
    int pieces = 8;
    int maxHeight = 8;
    int playerCount = 4;
    int maxPoints = 100;
    int maxItems = 0;
    int remainingItems = 0;
    int uniqueItemCount = 3;
    int remainingPoints = 0;
    int fatalBattleDifference = 4;
    bool claimEndsTurn = true;
    bool autoRoll = false;
    bool infiniteRolls = false;
    bool evenTurnCount = true;
    bool assistFirstTurn = false;
    std::string assistedColor = "magenta";
    int assistDiceCount = 10;
    int colorCount = 13;
    TeamColor teamColors[4] = {RED, BLUE, GREEN, YELLOW};

public:

    int GetPlayerCount() { return playerCount; }
    int GetPieces() { return pieces; }
    int GetRemainingPoints() { return remainingPoints; }
    int GetMaxPoints() { return maxPoints; }
    int GetMaxHeight() { return maxHeight; }
    int GetRemainingItems() { return remainingItems; }
    int GetMaxItems() { return maxItems; }
    int GetUniqueItemCount() { return uniqueItemCount; }
    int GetFatalBattleDifference() { return fatalBattleDifference; }
    bool GetClaimEndsTurn() { return claimEndsTurn; }
    bool GetAutoRoll() { return autoRoll; }
    bool GetEvenTurnCount() { return evenTurnCount; }
    bool GetInfiniteRolls() { return infiniteRolls; }
    bool GetAssistFirstTurn() { return assistFirstTurn; }
    std::string GetAssistedColor() { return assistedColor; }
    int GetAssistDiceCount() { return assistDiceCount; }
    void SetPlayerCount(int pc) { playerCount = pc; }
    void SetMaxPoints(int mp) { maxPoints = mp; }
    void SetPieces(int p) { pieces = p; }
    void SetMaxItems(int mi) { maxItems = mi; }
    void SetRemainingPoints(int rp) { remainingPoints = rp; }
    void SetRemainingItems(int ri) { remainingItems = ri; }
    void SetMaxHeight(int mh) { maxHeight = mh; }
    void SetFatalBattleDifference(int fbd) { fatalBattleDifference = fbd; }
    void SetClaimEndsTurn(bool cet) { claimEndsTurn = cet; }
    void SetAutoRoll(bool arm) { autoRoll = arm; }
    void SetInfiniteRolls(bool ir) { infiniteRolls = ir; }
    void SetAssistFirstTurn(bool aft) { assistFirstTurn = aft; }
    void SetAssistedColor(std::string ac) { assistedColor = ac; }
    void SetAssistDiceCount(int adc) { assistDiceCount = adc; }
    void CycleTeamColor(int i);
    bool ColorTaken(int i);
    TeamColor GetColor(int i);
};