#include "Player.h"
#include "GlobalVars.h"

Player::Player() {

}

void Player::SetScore(int s)
{
    score = s;
    if (scoreText != nullptr)
    {
        scoreText->SetTextContent((to_string(score)).c_str(), renderer);
    }
}