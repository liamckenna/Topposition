#include "Player.h"
#include "GlobalVars.h"
#include <algorithm>

Player::Player() {

}

void Player::SetScore(int s)
{
    score = s;
    if (scoreText != nullptr && firstText != nullptr && tieText != nullptr)
    {
        scoreText->SetTextContent((to_string(score)).c_str(), renderer);
        firstText->SetPosition(scoreText->GetPosition().first + scoreText->GetWidth(), firstText->GetPosition().second);
        tieText->SetPosition(scoreText->GetPosition().first + scoreText->GetWidth(), tieText->GetPosition().second);
    }
}

int Player::GetSoldierIndex(Piece* soldier)
{
    for (size_t i = 0; i < soldiers.size(); i++)
    {
        if (soldiers[i] == soldier)
        {
            return i;
        }
    }
    return -1;
}

int Player::GetPlayerIndex()
{
    for (size_t i = 0; i < players.size(); i++)
    {
        if (this == players[i])
        {
            return i;
        }
    }
    return -1;
}

void Player::RefreshColor()
{
    RefreshColorVars();
    RefreshColorDependentVars();
}

void Player::RefreshColorVars()
{
    switch(rules->GetColor(GetPlayerIndex()))
    {
        case RED:
            sdlColor = sdl_red;
            color = "red";
            break;
        case GREEN:
            sdlColor = sdl_green;
            color = "green";
            break;
        case BLUE:
            sdlColor = sdl_blue;
            color = "blue";
            break;
        case YELLOW:
            sdlColor = sdl_yellow;
            color = "yellow";
            break;
        case MAGENTA:
            sdlColor = sdl_magenta;
            color = "magenta";
            break;
        case CYAN:
            sdlColor = sdl_cyan;
            color = "cyan";
            break;
        case BLACK:
            sdlColor = sdl_black;
            color = "black";
            break;
        case WHITE:
            sdlColor = sdl_white;
            color = "white";
            break;
    }
}

void Player::RefreshColorDependentVars()
{
    turnText->SetColor(sdlColor, renderer);
    if (this == currentTurn)
    {
        SDL_SetTextureColorMod(currentPlayerCircle->GetTexture(), currentTurn->GetSDLColor().r / 1.5f, currentTurn->GetSDLColor().g / 1.5f, currentTurn->GetSDLColor().b / 1.5f);
    }
    for (int i = 0; i < soldiers.size(); i++)
    {
        Animation *oldFloat = soldiers[i]->animations["floatIdle"];
        Animation *oldSalute = soldiers[i]->animations["saluteIdle"];
        animations.erase(std::remove(animations.begin(), animations.end(), oldFloat), animations.end());
        animations.erase(std::remove(animations.begin(), animations.end(), oldSalute), animations.end());
        delete oldFloat;
        delete oldSalute;

        Animation *floatIdle = new Animation(textures[color + " piece float sheet"][0], surfaces[color + " piece float sheet"], 1, 12, {3, 4}, {48, 48});
        soldiers[i]->animations["floatIdle"] = floatIdle;

        Animation *saluteIdle = new Animation(textures[color + " piece salute sheet"][0], surfaces[color + " piece salute sheet"], 2, 24, {4, 6}, {48, 48});
        soldiers[i]->animations["saluteIdle"] = saluteIdle;

        if (soldiers[i]->GetOccupyingTerrain() != nullptr)
        {
            soldiers[i]->SetCurrentAnimation(saluteIdle);
        }
        else
        {
            soldiers[i]->SetCurrentAnimation(floatIdle);
        }

        soldierHeads[i]->SetTexture(textures[color + " player head"][0]);
        soldierHeads[i]->SetSurface(surfaces[color + " player head"]);
    }

    for (int i = 0; i < flags.size(); i++)
    {
        flags[i]->SetTexture(textures[color + " flag"][0]);
        flags[i]->SetSurface(surfaces[color + " flag"]);
    }
}

SDL_Color GetSDLColorFromTeamColor(TeamColor tc)
{
    switch(tc)
    {
        case RED:
            return sdl_red;
        case BLUE:
            return sdl_blue;
        case GREEN:
            return sdl_green;
        case YELLOW:
            return sdl_yellow;
        case MAGENTA:
            return sdl_magenta;
        case CYAN:
            return sdl_cyan;
        case BLACK:
            return sdl_black;
        case WHITE:
            return sdl_white;
    }
    return sdl_red;
}