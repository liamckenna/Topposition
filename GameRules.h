#pragma once
class GameRules {
    int pieces = 6;
    int maxHeight = 8;
    int playerCount = 4;
    int peakCount = 20;
    int maxPoints = 100;
    int maxItems = 10;
    int remainingItems = 10;
    int uniqueItemCount = 3;
    int remainingPoints = 100;
public:
    int GetPlayerCount() {return playerCount;}
    int GetPieces() {return pieces;}
    int GetRemainingPoints() {return remainingPoints;}
    int GetMaxPoints() {return maxPoints;}
    int GetMaxHeight() {return maxHeight;}
    int GetPeakCount() {return peakCount;}
    int GetRemainingItems() {return remainingItems;}
    int GetMaxItems() {return maxItems;}
    int GetUniqueItemCount() {return uniqueItemCount;}
    void SetPlayerCount(int pc) {playerCount = pc;}
    void SetMaxPoints(int mp) {maxPoints = mp;}
    void SetPieces(int p) {pieces = p;}
    void SetMaxItems(int mi) {maxItems = mi;}
    void SetRemainingPoints(int rp) {remainingPoints = rp;}
    void SetRemainingItems(int ri) {remainingItems = ri;}
    void SetMaxHeight(int mh) {maxHeight = mh;}
    void SetPeakCount(int pc) {peakCount = pc;}
};