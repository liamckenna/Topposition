#pragma once
#include <vector>
#include "GameObject.h"
#include <string>

class Peak;
class Piece;
class Item;
class UIElement;
class Text;


class Player {
    int score = 0;
    std::string name = "";
    std::string circleTextString = "";
    Text *turnText = nullptr;
    Text *scoreText = nullptr;
    Text *circleText = nullptr;
    Text *firstText = nullptr;
    Text *tieText = nullptr;
    std::string color;
    SDL_Color sdlColor;
public:
    std::vector<Peak*> peaks;
    std::vector<Piece*> soldiers;
    std::vector<Item*> inventory;
    std::vector<UIElement*> soldierHeads;
    std::vector<UIElement*> soldierHeadCrosses;
    Player();
    std::string GetName() {return name;}
    void SetName(std::string n) {name = n;}
    int GetScore() {return score;}
    void SetScore(int s);
    Text* GetTurnText() {return turnText;}
    void SetTurnText(Text* tt) {turnText = tt;}
    Text* GetScoreText() {return scoreText;}
    void SetScoreText(Text* st) {scoreText = st;}
    Text* GetFirstText() {return firstText;}
    void SetFirstText(Text* ft) {firstText = ft;}
    Text* GetTieText() {return tieText;}
    void SetTieText(Text* tt) {tieText = tt;}
    std::string GetColor() {return color;}
    void SetColor(std::string c) {color = c;}
    Text* GetCircleText() {return circleText;}
    void SetCircleText(Text* ct) {circleText = ct;}
    SDL_Color GetSDLColor() {return sdlColor;}
    void SetSDLColor(SDL_Color c) {sdlColor = c;}
    std::string GetCircleTextString() {return circleTextString;}
    void SetCircleTextString(std::string cts) {circleTextString = cts;}
    int GetSoldierIndex(Piece* soldier);
    int GetPlayerIndex();
};
