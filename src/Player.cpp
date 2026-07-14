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
        case BLACK:
            sdlColor = sdl_black;
            color = "black";
            break;
        case WHITE:
            sdlColor = sdl_white;
            color = "white";
            break;
        case AQUAMARINE:
            sdlColor = sdl_aquamarine;
            color = "aquamarine";
            break;
        case CORNFLOWER:
            sdlColor = sdl_cornflower;
            color = "cornflower";
            break;
        case FOREST:
            sdlColor = sdl_forest;
            color = "forest";
            break;
        case MELROSE:
            sdlColor = sdl_melrose;
            color = "melrose";
            break;
        case PURPLE:
            sdlColor = sdl_purple;
            color = "purple";
            break;
        case ORANGE:
            sdlColor = sdl_orange;
            color = "orange";
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
        Animation *oldFloatSalute = soldiers[i]->animations["floatSaluteIdle"];
        Animation *oldStand = soldiers[i]->animations["standIdle"];
        int off = oldFloat->GetFrame();
        int osf = oldSalute->GetFrame();
        int ofs = oldFloatSalute->GetFrame();
        int ost = oldStand->GetFrame();
        animations.erase(std::remove(animations.begin(), animations.end(), oldFloat), animations.end());
        animations.erase(std::remove(animations.begin(), animations.end(), oldSalute), animations.end());
        animations.erase(std::remove(animations.begin(), animations.end(), oldFloatSalute), animations.end());
        animations.erase(std::remove(animations.begin(), animations.end(), oldStand), animations.end());
        delete oldFloat;
        delete oldSalute;
        delete oldFloatSalute;
        delete oldStand;

        Animation *floatIdle = new Animation(textures[color + " float sheet"][0], surfaces[color + " float sheet"], 1, 12, {3, 4}, {48, 48}, off);
        soldiers[i]->animations["floatIdle"] = floatIdle;

        Animation *saluteIdle = new Animation(textures[color + " salute sheet"][0], surfaces[color + " salute sheet"], 2, 24, {4, 6}, {48, 48}, osf);
        soldiers[i]->animations["saluteIdle"] = saluteIdle;

        Animation *floatSaluteIdle = new Animation(textures[color + " float salute sheet"][0], surfaces[color + " float salute sheet"], 1, 12, {3, 4}, {48, 48}, ofs);
        soldiers[i]->animations["floatSaluteIdle"] = floatSaluteIdle;

        Animation *standIdle = new Animation(textures[color + " stand sheet"][0], surfaces[color + " stand sheet"], 2, 24, {4, 6}, {48, 48}, ost);
        soldiers[i]->animations["standIdle"] = standIdle;

        if (soldiers[i]->GetOccupyingTerrain() != nullptr)
        {
            if (this == currentTurn)
            {
                soldiers[i]->SetCurrentAnimation(saluteIdle);
            }
            else
            {
                soldiers[i]->SetCurrentAnimation(standIdle);
            }
        }
        else
        {
            if (this == currentTurn)
            {
                soldiers[i]->SetCurrentAnimation(floatSaluteIdle);
            }
            else
            {
                soldiers[i]->SetCurrentAnimation(floatIdle);
            }
        }

        soldierHeads[i]->SetTexture(textures[color + " head"][0]);
        soldierHeads[i]->SetSurface(surfaces[color + " head"]);
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
        case BLACK:
            return sdl_black;
        case WHITE:
            return sdl_white;
        case AQUAMARINE:
            return sdl_aquamarine;
        case CORNFLOWER:
            return sdl_cornflower;
        case FOREST:
            return sdl_forest;
        case MELROSE:
            return sdl_melrose;
        case PURPLE:
            return sdl_purple;
        case ORANGE:
            return sdl_orange;
        }
    return sdl_red;
}