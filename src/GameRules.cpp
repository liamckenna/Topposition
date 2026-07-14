#include "GameRules.h"
#include "GlobalVars.h"

void GameRules::CycleTeamColor(int i)
{
    if (i >= playerCount)
        return;

    do
    {
        int color = teamColors[i];
        if (color == colorCount)
        {
            color = 0;
        }
        teamColors[i] = TeamColor(color + 1);
    } while (ColorTaken(i));

    players[i]->RefreshColor();
}

bool GameRules::ColorTaken(int i)
{
    bool taken = false;
    for (int j = 0; j < playerCount; j++)
    {
        if (i == j)
            continue;
        else if (teamColors[i] == teamColors[j])
        {
            taken = true;
            break;
        }
    }
    return taken;
}

TeamColor GameRules::GetColor(int i)
{
    return teamColors[i];
}