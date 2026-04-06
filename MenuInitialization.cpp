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
    UIElement *logo = new UIElement("logo 1", textures["logo 1"][0], surfaces["logo 1"], true, false);
    uiElements.push_back(logo);
    gameObjects[0].push_back(logo);
    logo->SetScale((float)SCREEN_HEIGHT / 1152);
    logo->SetGlobalPosition(0, 0);

    UIElement *playButton = new UIElement("play", textures["play"][0], surfaces["play"], true, true);
    uiElements.push_back(playButton);
    gameObjects[0].push_back(playButton);
    playButton->SetScale((float)SCREEN_HEIGHT / 1152);
    playButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2);
    playButton->SetSelectable(true);

    // UIElement *settingsButton = new UIElement("settings", textures["settings"][0], surfaces["settings"], true, true);
    // uiElements.push_back(settingsButton);
    // gameObjects[0].push_back(settingsButton);
    // settingsButton->SetScale((float)SCREEN_HEIGHT / 1152);
    // settingsButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2 + (settingsButton->GetDimensions().second * settingsButton->GetScale()) - 20);

    UIElement *quitButton = new UIElement("quit", textures["quit"][0], surfaces["quit"], true, true);
    uiElements.push_back(quitButton);
    gameObjects[0].push_back(quitButton);
    quitButton->SetScale((float)SCREEN_HEIGHT / 1152);
    quitButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2 + ((quitButton->GetDimensions().second * quitButton->GetScale()) * 2));
};

void loadMenuBackground()
{
    UIElement *background = new UIElement("background", textures["background"][0], surfaces["background"], true, false);
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
        else if (uiElements[i]->GetName() == "reset button")
        {
            uiElements[i]->SetGlobalPosition(0, 0);
        }
        else if (uiElements[i]->GetName() == "dieOne")
        {
            uiElements[i]->SetGlobalPosition(SCREEN_WIDTH - (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale()) - 10, 10);
        }
        else if (uiElements[i]->GetName() == "dieTwo")
        {
            UIElement *die1 = nullptr;
            for (int j = 0; j < uiElements.size(); j++)
            {
                if (uiElements[j]->GetName() == "dieOne")
                {
                    die1 = uiElements[j];
                    break;
                }
            }
            uiElements[i]->SetGlobalPosition(SCREEN_WIDTH - ((die1->GetDimensions().first * die1->GetScale()) + 10) * 2, 10);
        }
        else if (uiElements[i]->GetName() == "movesLeftText")
        {
            uiElements[i]->SetGlobalPosition(0, SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale()));
        }
        else if (uiElements[i]->GetName() == "movesLeftCount")
        {
            uiElements[i]->SetGlobalPosition(0, SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale()));
        }
        else if (uiElements[i]->GetName() == "finish turn button")
        {
            uiElements[i]->SetGlobalPosition(SCREEN_WIDTH - (uiElements[i]->GetDimensions().first * uiElements[i]->GetScale()) - 10,
                                       SCREEN_HEIGHT - (uiElements[i]->GetDimensions().second * uiElements[i]->GetScale()) - 10);
        }
    }
}
