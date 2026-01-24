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
    logo->SetPosition(0, 0);

    UIElement *playButton = new UIElement("play", textures["play"][0], surfaces["play"], true, true);
    uiElements.push_back(playButton);
    gameObjects[0].push_back(playButton);
    playButton->SetScale((float)SCREEN_HEIGHT / 1152);
    playButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2);
    playButton->SetSelectable(true);

    UIElement *settingsButton = new UIElement("settings", textures["settings"][0], surfaces["settings"], true, true);
    // uiElements.push_back(settingsButton);
    // gameObjects[0].push_back(settingsButton);
    settingsButton->SetScale((float)SCREEN_HEIGHT / 1152);
    settingsButton->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 * 2 + (settingsButton->GetDimensions().second * settingsButton->GetScale()) - 20);

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
    background->SetPosition(0, 0);
}