#include "GameInitialization.h"


bool loadMap()
{

    //Loading success flag
    bool success = true;

    //Load splash image
    GameObject* water = new GameObject("water", textures["water"][0], surfaces["water"], false, true);


    water->SetPosition(0, 0);


    for (int i = 0; i < rules->GetMaxHeight() + 1; i++) {
        gameObjects.push_back(vector<GameObject *>());
        terrain.push_back(vector<Terrain*>());
        terrainOutlines.push_back(vector<GameObject*>());
        //generate map layers
    }

    gameObjects[0].push_back(water);
    rules->SetRemainingPoints(rules->GetMaxPoints());
    rules->SetRemainingItems(rules->GetMaxItems());
    while (rules->GetRemainingPoints() > 0) {
        GeneratePeak();
    }
    GroomTerrain();

    return success;
}

void loadGamePieces()
{
    gameObjects.push_back(vector<GameObject*>());
    for (int j = 0; j < rules->GetPlayerCount(); j++) {
        string color;
        string playerNumber;
        bool selectable;
        Player* player = new Player();
        players.push_back(player);
        switch(j) {
            case 0:
                color = "red";
                playerNumber = "playerOne";
                selectable = true;
                break;
            case 1:
                color = "green";
                playerNumber = "playerTwo";
                selectable = false;
                break;
            case 2:
                color = "blue";
                playerNumber = "playerThree";
                selectable = false;
                break;
            case 3:
                color = "yellow";
                playerNumber = "playerFour";
                selectable = false;
                break;
        }
        player->SetName(playerNumber);
        string flagName = playerNumber + " flag";
        for (int i = 0; i < peaks.size(); i++) {
            Piece* flag = new Piece(playerNumber, textures[flagName][0], surfaces[flagName], false);
            flag->SetScale(2);
            flag->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second - 40);
            pieces.push_back(flag);
            gameObjects[gameObjects.size() - 2].push_back(flag);
            peaks[i]->flags.push_back(flag);
            flag->SetPlayer(player);
        }
        string pieceName = playerNumber + " piece float";
        int x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
        int y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
        while (selectTerrain(x, y) != NULL || selectUI(x, y) != NULL) {
            x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
            y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
        }
        for (int i = 0; i < rules->GetPieces(); i++) {
            Piece* piece = new Piece(playerNumber, textures[pieceName][0], surfaces[pieceName], true);
            piece->SetScale(2);
            gameObjects[rules->GetMaxHeight() + 1].push_back(piece);
            piece->SetCenter(x, y);
            piece->SetDesignatedLocation(x, y);
            piece->SetSelectable(selectable);
            Animation* floatIdle = new Animation(textures[playerNumber + " piece float sheet"][0], surfaces[playerNumber + " piece float sheet"], 1, 12, {3, 4}, {48, 48});
            Animation* saluteIdle = new Animation(textures[playerNumber + " piece salute sheet"][0], surfaces[playerNumber + " piece salute sheet"], 2, 24, {4, 6}, {48, 48});
            animations.push_back(floatIdle);
            animations.push_back(saluteIdle);
            piece->SetCurrentAnimation(floatIdle);
            piece->animations["floatIdle"] = floatIdle;
            piece->animations["saluteIdle"] = saluteIdle;
            pieces.push_back(piece);
            player->soldiers.push_back(piece);
            piece->SetPlayer(player);
        }
    }
    SDL_Color White = {255, 255, 255};

    Text* currentTurnText = new Text("currentTurnText", "Fonts/yoster.ttf", White, 150, 0, 300, 50, 100, "Current Turn: ");
    text.push_back(currentTurnText);
    SDL_Color Red = {255, 0, 0};

    playerOneText = new Text("playerOneText", "Fonts/yoster.ttf", Red, 450, 0, 250, 50, 100, "Player One");
    text.push_back(playerOneText);
    if (rules->GetPlayerCount() > 0) players[0]->SetTurnText(playerOneText);

    SDL_Color Green = {0, 255, 0};

    playerTwoText = new Text("playerTwoText", "Fonts/yoster.ttf", Green, 450, 0, 250, 50, 100, "Player Two");
    text.push_back(playerTwoText);
    playerTwoText->SetRendered(false);

    if (rules->GetPlayerCount() > 1) players[1]->SetTurnText(playerTwoText);

    SDL_Color Blue = {0, 0, 255};

    playerThreeText = new Text("playerThreeText", "Fonts/yoster.ttf", Blue, 450, 0, 300, 50, 100, "Player Three");
    text.push_back(playerThreeText);
    playerThreeText->SetRendered(false);
    if (rules->GetPlayerCount() > 2) players[2]->SetTurnText(playerThreeText);

    SDL_Color Yellow = {255, 255, 0};

    playerFourText = new Text("playerFourText", "Fonts/yoster.ttf", Yellow, 450, 0, 275, 50, 100, "Player Four");
    text.push_back(playerFourText);
    playerFourText->SetRendered(false);
    if (rules->GetPlayerCount() > 3) players[3]->SetTurnText(playerFourText);
    currentTurn = players[0];

}

void loadUI() {

    UIElement* resetButton = new UIElement("reset button", textures["reset"][0], surfaces["reset"], true);
    uiElements.push_back(resetButton);
    gameObjects[gameObjects.size() - 1].push_back(resetButton);
    resetButton->SetPosition(0,0);
    UIElement* die1 = new UIElement("dieOne", textures["die 1"][0], surfaces["die 1"], true);
    die1->SetScale(0.1);
    uiElements.push_back(die1);
    gameObjects[gameObjects.size() - 1].push_back(die1);
    die1->SetPosition(SCREEN_WIDTH - (die1->GetDimensions().first * die1->GetScale() * die1->GetSize()) - 10, 10);
    UIElement* die2 = new UIElement("dieTwo", textures["die 2"][0], surfaces["die 2"], true);
    die2->SetScale(0.1);
    uiElements.push_back(die2);
    gameObjects[gameObjects.size() - 1].push_back(die2);
    die2->SetPosition(SCREEN_WIDTH - ((die1->GetDimensions().first * die1->GetScale() * die1->GetSize()) + 10 )*2, 10);
    UIElement* movesLeftText = new UIElement("movesLeftText", textures["moves left"][0], surfaces["moves left"], true);
    movesLeftText->SetScale(0.1);
    uiElements.push_back(movesLeftText);
    gameObjects[gameObjects.size() - 1].push_back(movesLeftText);
    movesLeftText->SetPosition(0, SCREEN_HEIGHT - (movesLeftText->GetDimensions().second * movesLeftText->GetScale() * movesLeftText->GetSize()));
    UIElement* movesLeftCount = new UIElement("movesLeftCount", textures["moves left 3"][0], surfaces["moves left 3"], true);
    movesLeftCount->SetScale(0.1);
    uiElements.push_back(movesLeftCount);
    gameObjects[gameObjects.size() - 1].push_back(movesLeftCount);
    movesLeftCount->SetPosition(0, SCREEN_HEIGHT - (movesLeftCount->GetDimensions().second * movesLeftCount->GetScale() * movesLeftCount->GetSize()));
    UIElement* finishTurnButton = new UIElement("finish turn button", textures["finish turn"][0], surfaces["finish turn"], true);
    finishTurnButton->SetScale(0.1);
    finishTurnButton->SetPosition(SCREEN_WIDTH - (finishTurnButton->GetDimensions().first * finishTurnButton->GetScale() * finishTurnButton->GetSize()) - 10,
                                  SCREEN_HEIGHT - (finishTurnButton->GetDimensions().second * finishTurnButton->GetScale() * finishTurnButton->GetSize()) - 10);
    uiElements.push_back(finishTurnButton);
    gameObjects[gameObjects.size() - 1].push_back(finishTurnButton);


    for(int i = 0; i < peaks.size(); i++) {
        UIElement* claimPeakButton = new UIElement("claim peak button", textures["claim peak"][0], surfaces["claim peak"], false, peaks[i]);
        claimPeakButton->SetScale(0.1);
        claimPeakButton->SetCenter(peaks[i]->GetCenter().first + 50, peaks[i]->GetCenter().second - 50);
        claimPeakButton->SetResizable(true);
        uiElements.push_back(claimPeakButton);
        gameObjects[gameObjects.size() - 1].push_back(claimPeakButton);
        peaks[i]->SetClaimNotif(claimPeakButton);

    }
    currentRoll = Roll();
    movesLeft = currentRoll;


}

void ResetMap() {
    for (int i = 0; i < peaks.size(); i++) {
        peaks[i] = nullptr;
    }

    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j] = nullptr;
            terrainOutlines[i][j] = nullptr;
        }
    }

    for (int i = 0; i < pieces.size(); i++) {
        pieces[i] = nullptr;
    }

    for (int i = 0; i < uiElements.size(); i++) {
        uiElements[i] = nullptr;
    }

    for (int i = 0; i < players.size(); i++) {
        for (int j = 0; j < players[i]->soldiers.size(); j++) {
            players[i]->soldiers[j] = nullptr;
        }
        for (int j = 0; j < players[i]->peaks.size(); j++) {
            players[i]->peaks[j] = nullptr;
        }
        for (int j = 0; j < players[i]->inventory.size(); j++) {
            players[i]->inventory[j] = nullptr;
        }
        delete players[i];
    }

    for (int i = 0; i < pixels.size(); i++) {
        delete pixels[i];
    }

    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            delete gameObjects[i][j];
        }
    }

    for (int i = 0; i < text.size(); i++) {
        delete text[i];
    }

    for (int i = 0; i < animations.size(); i++) {
        delete animations[i];
    }

    std::vector<Piece*> newPieces;
    std::vector<std::vector<Terrain*>> newTerrain;
    std::vector<std::vector<GameObject*>> newTerrainOutlines;
    std::vector<std::vector<GameObject*>> newGameObjects;
    std::vector<Peak*> newPeaks;
    std::vector<UIElement*> newUIElements;
    std::vector<Player*> newPlayers;
    std::vector<Pixel*> newPixels;
    std::vector<Text*> newText;
    std::vector<Animation*> newAnimations;
    animations = newAnimations;
    players = newPlayers;
    text = newText;
    pixels = newPixels;
    uiElements = newUIElements;
    pieces = newPieces;
    terrain = newTerrain;
    terrainOutlines = newTerrainOutlines;
    gameObjects = newGameObjects;
    peaks = newPeaks;

    loadMap();
    loadUI();
    loadGamePieces();
    GeneratePixels();

    renderPixels();
    GPU_LoadTarget(NULL);
}

void GeneratePixels() {


    for (int i = -500; i < SCREEN_WIDTH + 500; i++) {
        for (int j = -500; j < SCREEN_HEIGHT + 500; j++) {
            int x = i*2 + 1;
            int y = j*2 + 1;
            Terrain* currentTerrain = selectTerrain(x, y);
            if (currentTerrain == nullptr) continue;

            Pixel* pixel = new Pixel("pixel", textures["pixel"][0], surfaces["pixel"], false, true);
            pixel->SetHiddenTerrain(selectTerrain(x, y));

            //y += 2;
            Terrain* tempTerrain = selectTerrain(x, y);
            int width= 2;
            int height = 2;
            pixel->SetPosition(i*2, j*2);

            /*Terrain* topLeftTerrain = selectTerrain(i*4, j*4);
            Terrain* bottomRightTerrain = selectTerrain(i*4 + 4, j*4 + 4);
            if (topLeftTerrain == nullptr && bottomRightTerrain == nullptr) {
                pixel->SetOutline(false);
            } else if ((topLeftTerrain == nullptr && bottomRightTerrain != nullptr) ||
                (topLeftTerrain != nullptr && bottomRightTerrain == nullptr) ||
                (topLeftTerrain->GetLayer() != bottomRightTerrain->GetLayer())) {
                pixel->SetOutline(true);
            }*/

            /*while (tempTerrain != NULL && tempTerrain->GetLayer() == pixel->GetHiddenTerrain()->GetLayer() && !pixel->GetOutline()) {
                y += 4;
                tempTerrain = selectTerrain(x, y);
                height += 4;
                j++;
                if (tempTerrain != pixel->GetHiddenTerrain()) {
                    bool contin = false;
                    for (int i = 0; i < pixel->GetHiddenTerrain()->connectedTerrain.size(); i++) {
                        if (tempTerrain == pixel->GetHiddenTerrain()->connectedTerrain[i]) {
                            contin = true;
                            break;
                        }
                    }
                    if (!contin) break;
                }

            }*/

            pixel->SetWidth(width);
            pixel->SetHeight(height);

            SDL_Color pixelColor;
            if (pixel->GetHiddenTerrain()->GetLayer() == 1 && pixel->GetHiddenTerrain()->GetBiome() == "plains") {
                Uint8 randR = rand() % 56 + 200;
                Uint8 randG = randR - 8;
                Uint8 randB = randR - 50;
                pixelColor = {randR, randG, randB, 255};
            } else {
                Uint8 randR = rand() % 11;
                Uint8 randG = rand() % 11;
                Uint8 randB = rand() % 11;
                Uint8 randT = rand() % 2;
                if (pixel->GetHiddenTerrain()->GetBiome() == "plains") {
                    pixelColor = {static_cast<Uint8>(125 /rules->GetMaxHeight() * pixel->GetHiddenTerrain()->GetLayer() + (randR * randT)),
                                  static_cast<Uint8>(std::min(125 / rules->GetMaxHeight() * currentTerrain->GetLayer() * 2 + (randG * randT), 255)),
                                  static_cast<Uint8>(125 / rules->GetMaxHeight() * currentTerrain->GetLayer() + (randB * randT)), 255};
                } else if (pixel->GetHiddenTerrain()->GetBiome() == "stone") {
                    pixelColor = {static_cast<Uint8>(200/rules->GetMaxHeight() * pixel->GetHiddenTerrain()->GetLayer() + (randR * randT)),
                                  static_cast<Uint8>(200/rules->GetMaxHeight() * pixel->GetHiddenTerrain()->GetLayer() + (randG * randT)),
                                  static_cast<Uint8>(200 / rules->GetMaxHeight() * currentTerrain->GetLayer() + (randB * randT)), 255};
                } else if (pixel->GetHiddenTerrain()->GetBiome() == "glacier") {
                    pixelColor = {static_cast<Uint8>(185/rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1)),
                                  static_cast<Uint8>(std::min(200/rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + 100, 255)),
                                  static_cast<Uint8>(std::min(255 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + 140, 255)), 255};
                }
            }
            pixel->SetColor(pixelColor);
            pixels.push_back(pixel);
            //gameObjects[pixel->GetHiddenTerrain()->GetLayer()].push_back(pixel);
        }
    }
}


void renderPixels(){
    for (int i = 0; i < pixels.size(); i++) {
        pixels[i]->RenderGameObject(window, hoveringTerrain);
    }
}
