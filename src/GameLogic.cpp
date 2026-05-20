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
    for (int i = 0; i < text.size(); i++)
    {
        if (text[i]->GetName() == "movesLeftText")
        {
            std::pair<float, float> center = text[i]->GetCenter();
            text[i]->SetTextContent(to_string(movesLeft).c_str(), renderer);
            text[i]->SetCenter(center.first, center.second);
            break;
        }
    }
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
        turnCount++;
        if (turnCount == 10)
        {
            turnTallyNumText->SetPosition(turnTallyNumText->GetPosition().first - (turnTallyNumText->GetDimensions().first / 2), turnTallyNumText->GetPosition().second);
            turnTallyText->SetPosition(turnTallyText->GetPosition().first - (turnTallyNumText->GetDimensions().first / 2), turnTallyText->GetPosition().second);
        }
        turnTallyNumText->SetTextContent(to_string(turnCount).c_str(), renderer);
    }
    currentRoll = Roll();
    movesLeft = currentRoll;
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
        if (firstPlace != nullptr)
            GameFinished(firstPlace);
    }
}

void GameFinished(Player *winner)
{
    state = PAUSED;
    pState = WINNER;

    string textContent = "Player " + to_string(winner->GetPlayerIndex() + 1);
    textContent = textContent + " Wins!";

    std::cout << textContent << std::endl;

    playerWinsText->SetColor(winner->GetSDLColor(), renderer);
    playerWinsText->SetSize(playerWinsText->GetSize(), renderer);
    playerWinsText->SetTextContent(textContent.c_str(), renderer);
    playerWinsText->SetSize(playerWinsText->GetSize(), renderer);
}

void Tiebreaker()
{
}
