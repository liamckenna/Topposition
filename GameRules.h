#pragma once
class GameRules {

public:
    int pieces = 6;
    int maxHeight = 8;
    int peakCount = 20;
    int maxPoints = 100;
    int remainingPoints = 100;
    int GetPieces() {return pieces;}
    int GetRemainingPoints() {return remainingPoints;}
    int GetMaxPoints() {return maxPoints;}
    int GetMaxHeight() {return maxHeight;}
    int GetPeakCount() {return peakCount;}
    void SetMaxPoints(int mp) {maxPoints = mp;}
    void SetPieces(int p) {pieces = p;}
    void SetRemainingPoints(int rp) {remainingPoints = rp;}
    void SetMaxHeight(int mh) {maxHeight = mh;}
    void SetPeakCount(int pc) {peakCount = pc;}
};