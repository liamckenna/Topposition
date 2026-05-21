#include "GameLogic.h"

int Roll()
{
    int rollOne = (rand() % 6) + 1;
    string rollOneStr = "die " + to_string(rollOne);
    int rollTwo = (rand() % 6) + 1;
    string rollTwoStr = "die " + to_string(rollTwo);

    vector<SDL_Texture *> dieFaces;
    for (int i = 1; i <= 6; i++) {
        dieFaces.push_back(textures["die " + to_string(i)][0]);
    }

    diceAnimations.clear();

    Uint64 now = SDL_GetTicks();
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetName() == "dieOne")
        {
            diceAnimations.emplace_back(uiElements[i], dieFaces, textures[rollOneStr][0], now);
        }
        else if (uiElements[i]->GetName() == "dieTwo")
        {
            diceAnimations.emplace_back(uiElements[i], dieFaces, textures[rollTwoStr][0], now);
        }
    }
    movesLeft = rollOne + rollTwo;

    movesLeftText->SetRendered(false);

    UpdateMovesLeft();

    hasRolled = true;

    return movesLeft;
}

void RotateTurn()
{

    currentTurn->GetCircleText()->SetRendered(false);

    for (int i = 0; i < currentTurn->soldiers.size(); i++)
    {
        currentTurn->soldiers[i]->SetSelectable(false);
        currentTurn->soldierHeads[currentTurn->GetSoldierIndex(currentTurn->soldiers[i])]->SetRendered(false);
        currentTurn->soldierHeadCrosses[currentTurn->GetSoldierIndex(currentTurn->soldiers[i])]->SetRendered(false);
    }

    do
    {
        for (int i = 0; i < players.size(); i++)
        {
            if (players[i] == currentTurn)
            {
                if (i == players.size() - 1)
                {
                    currentTurn = players[0];
                }
                else
                {
                    currentTurn = players[i + 1];
                }
                break;
            }
        }
    } while (SoldierCount(currentTurn) <= 0);

    for (int i = 0; i < currentTurn->soldiers.size(); i++)
    {
        currentTurn->soldiers[i]->SetSelectable(true);

        currentTurn->soldierHeads[currentTurn->GetSoldierIndex(currentTurn->soldiers[i])]->SetRendered(true);
        if (currentTurn->soldiers[i]->IsDead())
        {
            currentTurn->soldierHeadCrosses[currentTurn->GetSoldierIndex(currentTurn->soldiers[i])]->SetRendered(true);
        }
        else
        {
            currentTurn->soldierHeadCrosses[currentTurn->GetSoldierIndex(currentTurn->soldiers[i])]->SetRendered(false);
        }
    }

    currentTurn->GetCircleText()->SetRendered(true);
    SDL_SetTextureColorMod(currentPlayerCircle->GetTexture(), currentTurn->GetSDLColor().r / 1.5f, currentTurn->GetSDLColor().g / 1.5f, currentTurn->GetSDLColor().b / 1.5f);
}

void UpdateMovesLeft()
{
    std::pair<float, float> center = movesLeftText->GetCenter();
    movesLeftText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
    movesLeftText->SetTextContent(to_string(movesLeft).c_str(), renderer);
    movesLeftText->SetCenter(center.first, center.second);
}

void FinishTurn()
{
    crown->SetRendered(false);
    RotateTurn();
    movesLeft = 0;
    UpdateMovesLeft();
    RefreshClaimNotifs();
    if (currentTurn == players[0])
    {
        if (rules->GetEvenTurnCount() && allPeaksClaimed)
        {
            UpdateScore();
            if (firstPlace != nullptr)
            {
                GameFinished(firstPlace);
            }
            else
            {
                BeginSuddenDeath();
            }
        }

        turnCount++;

        if (!suddenDeath && !lastTurn)
        {
            if (turnCount == 10)
            {
                turnTallyNumText->SetPosition(turnTallyNumText->GetPosition().first - (turnTallyNumText->GetDimensions().first / 2), turnTallyNumText->GetPosition().second);
                turnTallyText->SetPosition(turnTallyText->GetPosition().first - (turnTallyNumText->GetDimensions().first / 2), turnTallyText->GetPosition().second);
            }
            turnTallyNumText->SetTextContent(to_string(turnCount).c_str(), renderer);
        }
        
    }

    hasRolled = false;

    if (rules->GetAutoRollMoves())
    {
        currentRoll = Roll();
        movesLeft = currentRoll;
    }
    else
    {
        ClearRoll();
    }
}

void BeginSuddenDeath()
{
    lastTurn = false;
    SDL_Color Red = {255, 0, 0};
    
    std::pair<float, float> center = {turnTallyText->GetPosition().first + ((turnTallyText->GetWidth() + turnTallyNumText->GetWidth()) / 2), turnTallyText->GetCenter().second};
    if (lastTurn || suddenDeath)
    {
        center = {turnTallyText->GetCenter().first, turnTallyText->GetCenter().second};
    }
    turnTallyNumText->SetRendered(false);
    turnTallyText->SetColor(Red, renderer);
    string suddenDeathText = "Sudden Death!";
    turnTallyText->SetTextContent(suddenDeathText.c_str(), renderer);
    turnTallyText->SetSize(85 * (SCREEN_WIDTH / 3840.f), renderer);
    turnTallyText->SetCenter(center.first, center.second);

    suddenDeath = true;
}

void ClearRoll()
{
    currentRoll = 0;
    movesLeft = 0;
    string rollText = "Roll!";
    
    die1->SetTexture(textures["die 0"][0]);
    die2->SetTexture(textures["die 0"][0]);
    std::pair<float, float> center = movesLeftText->GetCenter();
    movesLeftText->SetSize(100 * (SCREEN_WIDTH / 3840.f), renderer);
    movesLeftText->SetTextContent(rollText.c_str(), renderer);
    movesLeftText->SetCenter(center.first, center.second);
}

void UpdateScore()
{

    int pointsLeft = rules->GetMaxPoints();
    for (int i = 0; i < players.size(); i++)
    {
        players[i]->SetScore(0);
        for (int j = 0; j < players[i]->peaks.size(); j++)
        {
            players[i]->SetScore(players[i]->GetScore() + players[i]->peaks[j]->GetLayer());
            pointsLeft -= players[i]->peaks[j]->GetLayer();
        }
    }
    firstPlace = nullptr;
    for (int i = 0; i < players.size(); i++)
    {
        players[i]->GetFirstText()->SetRendered(false);
        players[i]->GetTieText()->SetRendered(false);
        if (firstPlace == nullptr || players[i]->GetScore() > firstPlace->GetScore())
        {
            firstPlace = players[i];
        }
    }
    if (firstPlace->GetScore() != 0)
    {
        firstPlace->GetFirstText()->SetRendered(true);
    }
    bool tie = false;
    for (int i = 0; i < players.size(); i++)
    {
        if (players[i]->GetScore() == firstPlace->GetScore() && firstPlace->GetScore() != 0)
        {
            if (players[i] != firstPlace)
            {
                firstPlace->GetFirstText()->SetRendered(false);
                firstPlace->GetTieText()->SetRendered(true);
                players[i]->GetTieText()->SetRendered(true);
                tie = true;
            }
        }
    }
    if (tie)
    {
        firstPlace = nullptr;
    }
    if (pointsLeft == 0)
    {
        if (!rules->GetEvenTurnCount())
        {
            if (firstPlace != nullptr)
                GameFinished(firstPlace);
            else if (!suddenDeath)
                BeginSuddenDeath();
        }
        else if (!lastTurn)
        {
            BeginLastTurn();
        }
    }
    if (suddenDeath)
    {
        if (firstPlace != nullptr)
        {
            GameFinished(firstPlace);
        }
    }
}

void BeginLastTurn()
{
    SDL_Color Yellow = {255, 255, 0};

    std::pair<float, float> center = {turnTallyText->GetPosition().first + ((turnTallyText->GetWidth() + turnTallyNumText->GetWidth()) / 2), turnTallyText->GetCenter().second};
    if (lastTurn)
    {
        center = {turnTallyText->GetCenter().first, turnTallyText->GetCenter().second};
    }
    turnTallyNumText->SetRendered(false);
    turnTallyText->SetColor(Yellow, renderer);
    string lastTurnText = "Last Turn!";
    turnTallyText->SetSize(100 * (SCREEN_WIDTH / 3840.f), renderer);
    turnTallyText->SetTextContent(lastTurnText.c_str(), renderer);
    turnTallyText->SetCenter(center.first, center.second);

    lastTurn = true;
}

void GameFinished(Player *winner)
{
    state = PAUSED;
    pState = WINNER;

    string textContent = "Player " + to_string(winner->GetPlayerIndex() + 1);
    textContent = textContent + " Wins!";
    playerWinsText->SetColor(winner->GetSDLColor(), renderer);
    playerWinsText->SetSize(playerWinsText->GetSize(), renderer);
    playerWinsText->SetTextContent(textContent.c_str(), renderer);
    playerWinsText->SetSize(playerWinsText->GetSize(), renderer);
}

void Tiebreaker()
{
}
