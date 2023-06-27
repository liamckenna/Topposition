#pragma once
class GameRules {

public:
    int pieces = 6;
    int maxHeight = 8;
    int peakCount = 20;
    int GetPieces() {return pieces;}
    int GetMaxHeight() {return maxHeight;}
    int GetPeakCount() {return peakCount;}
    void SetPieces(int p) {pieces = p;}
    void SetMaxHeight(int mh) {maxHeight = mh;}
    void SetPeakCount(int pc) {peakCount = pc;}
};