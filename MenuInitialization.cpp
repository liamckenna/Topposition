#include "MenuInitialization.h"

void LoadMenu()
{
    ResetMap();
    gameObjects.push_back(vector<GameObject *>());
    loadMenuBackground();
    loadMenuUI();
}

void loadMenuUI()
{
    UIElement *logo = new UIElement("logo 1", textures["logo 1"][0], surfaces["logo 1"], true, false, renderer);
    uiElements.push_back(logo);
    gameObjects[0].push_back(logo);
    logo->SetScale((float)SCREEN_HEIGHT / 1152);
    logo->SetGlobalPosition(0, 0);

    UIElement *playButton = new UIElement("play", textures["play"][0], surfaces["play"], true, true, renderer);
    uiElements.push_back(playButton);
    gameObjects[0].push_back(playButton);
    playButton->SetScale((float)SCREEN_HEIGHT / 1152);
    playButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2);
    playButton->SetSelectable(true);

    UIElement *quitButton = new UIElement("quit", textures["quit"][0], surfaces["quit"], true, true, renderer);
    uiElements.push_back(quitButton);
    gameObjects[0].push_back(quitButton);
    quitButton->SetScale((float)SCREEN_HEIGHT / 1152);
    quitButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2 + ((quitButton->GetDimensions().second * quitButton->GetScale()) * 2));
};

void loadMenuBackground()
{
    UIElement *background = new UIElement("background", textures["background"][0], surfaces["background"], true, false, renderer);
    uiElements.push_back(background);
    gameObjects[0].push_back(background);
    background->SetScale((float)SCREEN_HEIGHT / 1152);
    background->SetGlobalPosition(0, 0);
}


void updateUIElementPositions()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        if (uiElements[i]->GetName() == "logo 1")
        {
            uiElements[i]->SetScale((float)SCREEN_HEIGHT / 1152);
            uiElements[i]->SetGlobalPosition(0, 0);
        }
        else if (uiElements[i]->GetName() == "play")
        {
            uiElements[i]->SetScale((float)SCREEN_HEIGHT / 1152);
            uiElements[i]->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2);
        }
        else if (uiElements[i]->GetName() == "quit")
        {
            uiElements[i]->SetScale((float)SCREEN_HEIGHT / 1152);
            uiElements[i]->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2 + ((uiElements[i]->GetDimensions().second * uiElements[i]->GetScale()) * 2));
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
            Text *turnText = nullptr;
            Text* endText = nullptr;
            for (int j = 0; j < text.size(); j++)
            {
                if (text[j]->GetName() == "turnText")
                {
                    turnText = text[j];
                }
                else if (text[j]->GetName() == "endText")
                {
                    endText = text[j];
                    break;
                }
            }
            int textSize = 75 * (SCREEN_WIDTH / 3840.f);
            turnText->SetSize(textSize, renderer);
            endText->SetSize(textSize, renderer);
            turnText->SetPosition((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 8), SCREEN_HEIGHT - turnText->GetHeight() - (SCREEN_HEIGHT / 40));
            turnText->SetCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), turnText->GetCenter().second);
            endText->SetPosition((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 8) + ((turnText->GetWidth() - endText->GetWidth()) / 2), SCREEN_HEIGHT - turnText->GetHeight() - endText->GetHeight() - (SCREEN_HEIGHT / 40));
            endText->SetCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), endText->GetCenter().second);
            uiElements[i]->SetScale(0.8f * (SCREEN_WIDTH / 3840.f));
            uiElements[i]->SetGlobalCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), endText->GetPosition().second - (SCREEN_HEIGHT / 200) - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale() / 2));
        }
    }
    UpdateTextElementPositions();
}

void UpdateTextElementPositions()
{
    if (turnTallyText == nullptr || turnTallyNumText == nullptr)
    {
        return;
    }

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


    std::string peaksLeftString = "peaks left: " + to_string(unclaimedPeakCount);
    peaksLeftText->SetTextContent(peaksLeftString.c_str(), renderer);
    peaksLeftText->SetSize(65 * (SCREEN_WIDTH / 3840.f), renderer);
    peaksLeftText->SetPosition(x, y);

    movesLeftText->SetTextContent("11", renderer);
    movesLeftText->SetSize(150 * (SCREEN_WIDTH / 3840.f), renderer);
    x = (SCREEN_WIDTH / 2) - (movesLeftText->GetWidth() / 2);
    y = SCREEN_HEIGHT - movesLeftText->GetHeight() - (SCREEN_HEIGHT / 9);
    movesLeftText->SetPosition(x, y);
    std::pair<float, float> center = movesLeftText->GetCenter();
    movesLeftText->SetTextContent(to_string(movesLeft).c_str(), renderer);
    movesLeftText->SetCenter(center.first, center.second);

    int textSize = 125 * (SCREEN_WIDTH / 3840.f);
    y = SCREEN_HEIGHT / 40;

    for (int i = 0; i < rules->GetPlayerCount(); i++)
    {
        x = SCREEN_WIDTH / 80;
        if (i > 0)
        {
            y += players[0]->GetTurnText()->GetDimensions().second;
        }
        players[i]->GetTurnText()->SetTextContent(("P" + to_string(i + 1) + ":").c_str(), renderer);
        players[i]->GetTurnText()->SetSize(textSize, renderer);
        players[i]->GetScoreText()->SetTextContent("0", renderer);
        players[i]->GetScoreText()->SetSize(textSize, renderer);
        players[i]->GetCircleText()->SetTextContent(("P" + to_string(i + 1)).c_str(), renderer);
        players[i]->GetCircleText()->SetSize(textSize, renderer);
        players[i]->GetTurnText()->SetPosition(x, y);
        x += players[i]->GetTurnText()->GetWidth() + (SCREEN_WIDTH / 160);
        players[i]->GetScoreText()->SetPosition(x, y);
        players[i]->GetCircleText()->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        players[i]->GetScoreText()->SetTextContent(to_string(players[i]->GetScore()).c_str(), renderer);
    }
}