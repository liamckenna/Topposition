#pragma once
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
};