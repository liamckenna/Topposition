#pragma once
#include <vector>
#include "GameObject.h"
#include <string>

class Peak;
class Piece;
class Item;
class Text;


class Player {
    int score = 0;
    std::string name = "";
    Text* turnText;

public:
    std::vector<Peak*> peaks;
    std::vector<Piece*> soldiers;
    std::vector<Item*> inventory;
    Player();
    std::string GetName() {return name;}
    void SetName(std::string n) {name = n;}
    int GetScore() {return score;}
    void SetScore(int s) {score = s;}
    Text* GetTurnText() {return turnText;}
    void SetTurnText(Text* tt) {turnText = tt;}

};
