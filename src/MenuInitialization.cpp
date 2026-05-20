#include "MenuInitialization.h"

void LoadMenu()
{
    state = MAIN_MENU;
    ResetMap();
    gameObjects.push_back(vector<GameObject *>());
    loadMenuBackground();
    loadMenuUI();
}

void loadMenuUI()
{
    SDL_Color White = {255, 255, 255};

    UIElement *logo = new UIElement("Logo", textures["Logo"][0], surfaces["Logo"], true, false, renderer, MAIN_MENU);
    uiElements.push_back(logo);
    gameObjects[0].push_back(logo);
    logo->SetScale(20 * (SCREEN_WIDTH / 3840.f));
    logo->SetGlobalCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
    logo->SetRenderShadow(false);
    logo->SetShadowOffset(30, 30);

    int textSize = 250 * (SCREEN_WIDTH / 3840.f);
    Text *playButtonText = new Text("playButtonText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Play");
    playButtonText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 6);
    playButtonText->SetRenderShadow(false);
    playButtonText->SetSelectable(true);
    playButtonText->SetGameStateContext(MAIN_MENU);
    playButtonText->SetShadowOffset(30, 30);
    text.push_back(playButtonText);
    

    textSize = 175 * (SCREEN_WIDTH / 3840.f);
    Text *quitButtonText = new Text("quitButtonText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Quit");
    quitButtonText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8 + SCREEN_HEIGHT / 4);
    quitButtonText->SetRenderShadow(false);
    quitButtonText->SetSelectable(true);
    quitButtonText->SetGameStateContext(MAIN_MENU);
    quitButtonText->SetShadowOffset(30, 30);
    text.push_back(quitButtonText);
};

void loadMenuBackground()
{
    UIElement *background = new UIElement("background", textures["background"][0], surfaces["background"], true, false, renderer, MAIN_MENU);
    uiElements.push_back(background);
    gameObjects[0].push_back(background);
    background->SetScale((float)SCREEN_HEIGHT / 1152);
    background->SetGlobalPosition(0, 0);
}


void updateUIElementPositions()
{
    std::cout << "Updating UI element positions" << std::endl;
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetName() == "Logo")
        {
            uiElements[i]->SetScale(20 * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
        }
        else if (uiElements[i]->GetName() == "background")
        {
            uiElements[i]->SetScale((float)SCREEN_HEIGHT / 1152);
            uiElements[i]->SetGlobalPosition(0, 0);
        }
        else if (uiElements[i]->GetName() == "currentPlayerCircle")
        {
            uiElements[i]->SetScale(1.f * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter((SCREEN_WIDTH / 2) - (SCREEN_WIDTH / 7.50f), SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2) - (SCREEN_HEIGHT / 40));
        }
        else if (uiElements[i]->GetName() == "opposingPlayerCircle")
        {
            uiElements[i]->SetScale(1.f * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2) - (SCREEN_HEIGHT / 40));
        }
        else if (uiElements[i]->GetName() == "dieOne")
        {
            uiElements[i]->SetScale(0.15 * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter((SCREEN_WIDTH / 2) - (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale() / 1.25f), SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2) - (SCREEN_HEIGHT / 40));
        }
        else if (uiElements[i]->GetName() == "dieTwo")
        {
            uiElements[i]->SetScale(0.15 * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter((SCREEN_WIDTH / 2) + (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale() / 1.25f), SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2) - (SCREEN_HEIGHT / 40));
        }
        else if (uiElements[i]->GetName() == "endTurnArrow")
        {
            int textSize = 75 * (SCREEN_WIDTH / 3840.f);
            turnText->SetSize(textSize, renderer);
            endText->SetSize(textSize, renderer);
            turnText->SetPosition((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 8), SCREEN_HEIGHT - turnText->GetHeight() - (SCREEN_HEIGHT / 40));
            turnText->SetCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), turnText->GetCenter().second);
            endText->SetPosition((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 8) + ((turnText->GetWidth() - endText->GetWidth()) / 2), SCREEN_HEIGHT - turnText->GetHeight() - endText->GetHeight() - (SCREEN_HEIGHT / 40));
            endText->SetCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), endText->GetCenter().second);
            uiElements[i]->SetScale(0.8f * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), endText->GetPosition().second - (SCREEN_HEIGHT / 200) - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 3));
        }
        else if (uiElements[i]->GetName() == "fatalAttackExclamation")
        {
            uiElements[i]->SetScale(8 * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalPosition(die2->GetGlobalCenter().first + (die2->GetDimensions().first * die2->GetScale() / 2) + (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale()), die2->GetGlobalCenter().second - (die2->GetDimensions().second * die2->GetScale() / 2) - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2));
}
        else if (uiElements[i]->GetName() == "fatalDefenseExclamation")
        {
            uiElements[i]->SetScale(8 * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalPosition(die1->GetGlobalCenter().first - (die1->GetDimensions().first * die1->GetScale() / 2) - (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale()) - (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale()), die1->GetGlobalCenter().second - (die1->GetDimensions().second * die1->GetScale() / 2) - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2));
        }
    }
    if (crown)
    {
        crown->SetScale(8 * (SCREEN_WIDTH / 3840.f));
        if (crown->GetRendered())
        {
            if (crown->GetRotation() == -45)
            {
                crown->SetGlobalPosition(die1->GetGlobalCenter().first - (die1->GetDimensions().first * die1->GetScale() / 2) - (crown->GetDimensions().first * crown->GetScale()), die1->GetGlobalCenter().second - (die1->GetDimensions().second * die1->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
            }
            else
            {
                crown->SetGlobalPosition(die2->GetGlobalCenter().first + (die2->GetDimensions().first * die2->GetScale() / 2), die2->GetGlobalCenter().second - (die2->GetDimensions().second * die2->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
            }
        }
    }
    UpdateTextElementPositions();
}

void UpdateTextElementPositions()
{
    std::cout << "Updating text element positions" << std::endl;
    switch (state)
    {
        case MAIN_MENU:
            for (int i = 0; i < text.size(); i++)
            {
                if (text[i]->GetName() == "playButtonText")
                {
                    text[i]->SetSize(250 * (SCREEN_WIDTH / 3840.f), renderer);
                    text[i]->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 6);
                }
                else if (text[i]->GetName() == "settings")
                {
                    text[i]->SetSize(250 * (SCREEN_WIDTH / 3840.f), renderer);
                    text[i]->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2);
                }
                else if (text[i]->GetName() == "quitButtonText")
                {
                    text[i]->SetSize(175 * (SCREEN_WIDTH / 3840.f), renderer);
                    text[i]->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8 + SCREEN_HEIGHT / 4);
                }
            }
        case GAME:
        case PAUSED:
        {
            std::cout << "Updating turnTallyText & turnTallyNumText" << std::endl;
            turnTallyText->SetTextContent("Turn 80", renderer);
            turnTallyText->SetSize(125 * (SCREEN_WIDTH / 3840.f), renderer);
            turnTallyText->SetPosition(SCREEN_WIDTH - turnTallyText->GetWidth() - SCREEN_WIDTH / 80, SCREEN_HEIGHT / 40);

            
            turnTallyNumText->SetTextContent("1", renderer);
            turnTallyNumText->SetSize(125 * (SCREEN_WIDTH / 3840.f), renderer);
            turnTallyNumText->SetPosition(turnTallyText->GetPosition().first + turnTallyText->GetWidth() - (turnTallyNumText->GetWidth() * 1.5), SCREEN_HEIGHT / 40);

            int x = turnTallyText->GetPosition().first + (SCREEN_WIDTH / 160);
            int y = turnTallyText->GetPosition().second + turnTallyText->GetHeight() + (SCREEN_HEIGHT / 200);

            turnTallyText->SetPosition(turnTallyText->GetPosition().first + (turnTallyNumText->GetWidth() / 2), turnTallyText->GetPosition().second);
            turnTallyText->SetTextContent("Turn", renderer);
            if (turnCount >= 10)
            {
                turnTallyNumText->SetPosition(turnTallyNumText->GetPosition().first - (turnTallyNumText->GetDimensions().first / 2), turnTallyNumText->GetPosition().second);
                turnTallyText->SetPosition(turnTallyText->GetPosition().first - (turnTallyNumText->GetDimensions().first / 2), turnTallyText->GetPosition().second);
            }
            turnTallyNumText->SetTextContent(to_string(turnCount).c_str(), renderer);

            std::cout << "Updating peaksLeftText" << std::endl;
            std::string peaksLeftString = "peaks left: " + to_string(unclaimedPeakCount);
            peaksLeftText->SetTextContent(peaksLeftString.c_str(), renderer);
            peaksLeftText->SetSize(65 * (SCREEN_WIDTH / 3840.f), renderer);
            peaksLeftText->SetPosition(x, y);

            std::cout << "Updating movesLeftText" << std::endl;
            movesLeftText->SetTextContent("11", renderer);
            movesLeftText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
            x = (SCREEN_WIDTH / 2) - (movesLeftText->GetWidth() / 2);
            y = SCREEN_HEIGHT - movesLeftText->GetHeight() - (SCREEN_HEIGHT / 9);
            movesLeftText->SetPosition(x, y);
            std::pair<float, float> center = movesLeftText->GetCenter();
            movesLeftText->SetTextContent(to_string(movesLeft).c_str(), renderer);
            movesLeftText->SetCenter(center.first, center.second);

            std::cout << "Updating pausedText" << std::endl;
            pausedText->SetSize(200 * (SCREEN_WIDTH / 3840.f), renderer);
            pausedText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10);

            std::cout << "Updating resetMapText" << std::endl;
            resetMapText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
            resetMapText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8);

            std::cout << "Updating mainExitToMainMenuText" << std::endl;
            mainExitToMainMenuText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
            mainExitToMainMenuText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8 + SCREEN_HEIGHT / 10);
            
            playerWinsText->SetSize(200 * (SCREEN_WIDTH / 3840.f), renderer);
            playerWinsText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10);

            playAgainText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
            playAgainText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8);

            winnerExitToMainMenuText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
            winnerExitToMainMenuText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8 + SCREEN_HEIGHT / 10);

            int textSize = 125 * (SCREEN_WIDTH / 3840.f);
            y = SCREEN_HEIGHT / 40;

            for (int i = 0; i < rules->GetPlayerCount(); i++)
            {
                x = SCREEN_WIDTH / 80;
                if (i > 0)
                {
                    y += players[0]->GetTurnText()->GetDimensions().second;
                }
                std::cout << "Updating GetTurnText for Player " << i + 1 << std::endl;
                players[i]->GetTurnText()->SetTextContent(("P" + to_string(i + 1) + ":").c_str(), renderer);
                players[i]->GetTurnText()->SetSize(textSize, renderer);
                std::cout << "Updating GetScoreText for Player " << i + 1 << std::endl;
                players[i]->GetScoreText()->SetTextContent("0", renderer);
                players[i]->GetScoreText()->SetSize(textSize, renderer);
                textSize = 75 * (SCREEN_WIDTH / 3840.f);
                std::cout << "Updating GetFirstText for Player " << i + 1 << std::endl;
                players[i]->GetFirstText()->SetTextContent("1st!", renderer);
                players[i]->GetFirstText()->SetSize(textSize, renderer);
                std::cout << "Updating GetTieText for Player " << i + 1 << std::endl;
                players[i]->GetTieText()->SetTextContent("Tie!", renderer);
                players[i]->GetTieText()->SetSize(textSize, renderer);
                textSize = 125 * (SCREEN_WIDTH / 3840.f);
                std::cout << "Updating GetCircleText for Player " << i + 1 << std::endl;
                players[i]->GetCircleText()->SetTextContent(("P" + to_string(i + 1)).c_str(), renderer);
                players[i]->GetCircleText()->SetSize(textSize, renderer);
                std::cout << "Updating GetTurnText for Player " << i + 1 << std::endl;
                players[i]->GetTurnText()->SetPosition(x, y);
                x += players[i]->GetTurnText()->GetWidth() + (SCREEN_WIDTH / 160);
                std::cout << "Updating GetScoreText for Player " << i + 1 << std::endl;
                players[i]->GetScoreText()->SetPosition(x, y);
                std::cout << "Updating GetCircleText for Player " << i + 1 << std::endl;
                players[i]->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
                std::cout << "Updating GetScoreText for Player " << i + 1 << std::endl;
                players[i]->GetScoreText()->SetTextContent(to_string(players[i]->GetScore()).c_str(), renderer);
                x += players[i]->GetScoreText()->GetWidth();
                std::cout << "Updating GetFirstText for Player " << i + 1 << std::endl;
                players[i]->GetFirstText()->SetPosition(x, y - (players[i]->GetFirstText()->GetHeight() / 4));
                std::cout << "Updating GetTieText for Player " << i + 1 << std::endl;
                players[i]->GetTieText()->SetPosition(x, y - (players[i]->GetTieText()->GetHeight() / 4));
                for (int j = 0; j < players[i]->soldiers.size(); j++)
                {
                    players[i]->soldierHeads[j]->SetScale(2.f * (SCREEN_WIDTH / 3840.f));
                    players[i]->soldierHeadCrosses[j]->SetScale(2.f * (SCREEN_WIDTH / 3840.f));
                    players[i]->soldierHeads[j]->SetPosition(peaksLeftText->GetPosition().first + (j * (players[i]->soldierHeads[j]->GetDimensions().first * players[i]->soldierHeads[j]->GetScale() + SCREEN_WIDTH / 600)) - SCREEN_WIDTH / 350, peaksLeftText->GetPosition().second + peaksLeftText->GetHeight() + SCREEN_HEIGHT / 100);
                    players[i]->soldierHeadCrosses[j]->SetPosition(peaksLeftText->GetPosition().first + (j * (players[i]->soldierHeadCrosses[j]->GetDimensions().first * players[i]->soldierHeadCrosses[j]->GetScale() + SCREEN_WIDTH / 600)) - SCREEN_WIDTH / 350, peaksLeftText->GetPosition().second + peaksLeftText->GetHeight() + SCREEN_HEIGHT / 100);
                }
            }
            RefreshShadows();
            break;
        }
        default:
            break;
    }
}