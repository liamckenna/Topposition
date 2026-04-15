#include "GameInitialization.h"

void ResetMap()
{
    for (int i = 0; i < peaks.size(); i++)
    {
        peaks[i] = nullptr;
    }

    for (int i = 0; i < terrain.size(); i++)
    {
        for (int j = 0; j < terrain[i].size(); j++)
        {
            terrain[i][j] = nullptr;
        }
    }

    for (int i = 0; i < pieces.size(); i++)
    {
        pieces[i] = nullptr;
    }

    for (int i = 0; i < uiElements.size(); i++)
    {
        uiElements[i] = nullptr;
    }

    for (int i = 0; i < players.size(); i++)
    {
        for (int j = 0; j < players[i]->soldiers.size(); j++)
        {
            players[i]->soldiers[j] = nullptr;
        }
        for (int j = 0; j < players[i]->peaks.size(); j++)
        {
            players[i]->peaks[j] = nullptr;
        }
        for (int j = 0; j < players[i]->inventory.size(); j++)
        {
            players[i]->inventory[j] = nullptr;
        }
        delete players[i];
    }

    for (int i = 0; i < pixels.size(); i++)
    {
        delete pixels[i];
    }

    ocean.clear();

    for (int i = 0; i < gameObjects.size(); i++)
    {
        for (int j = 0; j < gameObjects[i].size(); j++)
        {
            delete gameObjects[i][j];
        }
    }

    for (int i = 0; i < text.size(); i++)
    {
        delete text[i];
    }

    for (int i = 0; i < animations.size(); i++)
    {
        delete animations[i];
    }

    std::vector<Piece *> newPieces;
    std::vector<std::vector<Terrain *>> newTerrain;
    std::vector<std::vector<GameObject *>> newGameObjects;
    std::vector<Peak *> newPeaks;
    std::vector<UIElement *> newUIElements;
    std::vector<Player *> newPlayers;
    std::vector<Pixel *> newPixels;
    std::vector<Text *> newText;
    std::vector<std::vector<OceanTile *>> newOcean;
    std::vector<Animation *> newAnimations;
    animations = newAnimations;
    players = newPlayers;
    text = newText;
    ocean = newOcean;
    pixels = newPixels;
    uiElements = newUIElements;
    pieces = newPieces;
    terrain = newTerrain;
    gameObjects = newGameObjects;
    peaks = newPeaks;
    if (state == GAME)
    {
        state = LOADING;
        loadGame();
        state = GAME;
    }
}

void loadGame()
{
    cameraZoom = 1.f;
    cameraPosition = {0, 0};
    worldResolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
    turnCount = 1;

    std::cout << "Loading Map..." << std::endl;
    loadMap();
    std::cout << "Loading Game Pieces..." << std::endl;
    loadGamePieces();
    std::cout << "Loading Text..." << std::endl;
    loadText();
    std::cout << "Loading UI..." << std::endl;
    loadUI();
    std::cout << "Generating Pixels..." << std::endl;
    GeneratePixels();
    std::cout << "Generating Ocean..." << std::endl;
    GenerateOcean();
    std::cout << "Rendering Pixels..." << std::endl;
    renderPixels();

    SDL_SetRenderTarget(renderer, NULL);

    std::cout << "Game Loaded!" << std::endl;
    gameStartTime = SDL_GetTicks();
}

bool loadMap()
{

    bool success = true;

    GameObject *water = new GameObject("water", textures["water"][0], surfaces["water"], false, true);

    water->SetGlobalPosition(0, 0);

    for (int i = 0; i < rules->GetMaxHeight() + 1; i++)
    {
        gameObjects.push_back(vector<GameObject *>());
        terrain.push_back(vector<Terrain *>());
    }

    gameObjects[0].push_back(water);
    rules->SetRemainingPoints(rules->GetMaxPoints());
    rules->SetRemainingItems(rules->GetMaxItems());
    while (rules->GetRemainingPoints() > 0)
    {
        GeneratePeak();
    }
    GroomTerrain();

    unclaimedPeakCount = peaks.size();

    return success;
}

void loadGamePieces()
{
    gameObjects.push_back(vector<GameObject *>());
    for (int j = 0; j < rules->GetPlayerCount(); j++)
    {
        string color;
        string playerNumber;
        SDL_Color sdlColor;
        bool selectable;
        Player *player = new Player();
        players.push_back(player);
        switch (j)
        {
        case 0:
            color = "red";
            playerNumber = "playerOne";
            selectable = true;
            sdlColor = {255, 0, 0};
            break;
        case 1:
            color = "green";
            playerNumber = "playerTwo";
            selectable = false;
            sdlColor = {0, 255, 0};
            break;
        case 2:
            color = "blue";
            playerNumber = "playerThree";
            selectable = false;
            sdlColor = {0, 0, 255};
            break;
        case 3:
            color = "yellow";
            playerNumber = "playerFour";
            selectable = false;
            sdlColor = {255, 255, 0};
            break;
        }
        player->SetName(playerNumber);
        player->SetColor(color);
        player->SetSDLColor(sdlColor);
        string flagName = playerNumber + " flag";
        for (int i = 0; i < peaks.size(); i++)
        {
            Piece *flag = new Piece(playerNumber, textures[flagName][0], surfaces[flagName], false);
            flag->SetScale(3);
            flag->SetBottomMiddle(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);

            pieces.push_back(flag);
            gameObjects[gameObjects.size() - 2].push_back(flag);
            peaks[i]->flags.push_back(flag);
            flag->SetPlayer(player);
        }
        string pieceName = playerNumber + " piece float";

        for (int i = 0; i < rules->GetPieces(); i++)
        {
            int x;
            int y;

            do
            {
                x = (rand() % (int)(MAP_WIDTH * 2 / 9));
                y = (rand() % (int)(MAP_HEIGHT * 2 / 9));
                switch (j % 4)
                {
                case 0:
                    x += MAP_WIDTH / 9 * 8;
                    y -= MAP_HEIGHT / 9 * 2;
                    break;
                case 1:
                    x += MAP_WIDTH / 9 * 8;
                    y += MAP_HEIGHT / 9 * 18;
                    break;
                case 2:
                    x -= MAP_WIDTH / 9 * 2;
                    y += MAP_HEIGHT / 9 * 8;
                    break;
                case 3:
                    x += MAP_WIDTH / 9 * 18;
                    y += MAP_HEIGHT / 9 * 8;
                    break;
                default:
                    x = (rand() % (int)(MAP_WIDTH * 3)) - MAP_WIDTH / 4;
                    y = (rand() % (int)(MAP_HEIGHT * 3)) - MAP_HEIGHT / 4;
                }
            } while (selectTerrain(x, y, false) != NULL || selectUI(x, y, false) != NULL);

            Piece *piece = new Piece(pieceName + " " + std::to_string(i), textures[pieceName][0], surfaces[pieceName], true);
            piece->SetScale(3);
            gameObjects[rules->GetMaxHeight() + 1].push_back(piece);
            piece->SetBottomMiddle(x, y);
            piece->SetDesignatedLocation(x, y);
            piece->SetSelectable(selectable);
            Animation *floatIdle = new Animation(textures[playerNumber + " piece float sheet"][0], surfaces[playerNumber + " piece float sheet"], 1, 12, {3, 4}, {48, 48});
            Animation *saluteIdle = new Animation(textures[playerNumber + " piece salute sheet"][0], surfaces[playerNumber + " piece salute sheet"], 2, 24, {4, 6}, {48, 48});
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

    currentTurn = players[0];
}

void loadText()
{
    SDL_Color White = {255, 255, 255};
    SDL_Color Red = {255, 0, 0};
    SDL_Color Green = {0, 255, 0};
    SDL_Color Blue = {0, 0, 255};
    SDL_Color Yellow = {255, 255, 0};

    
    int textSize = 125;

    turnTallyText = new Text("turnCountText", "Fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Turn 80");

    int x = SCREEN_WIDTH - turnTallyText->GetWidth() - 50;
    int y = 50;

    turnTallyText->SetPosition(x, y);
    text.push_back(turnTallyText);    

    turnTallyNumText = new Text("turnTallyNumText", "Fonts/yoster.ttf", White, x, y, textSize, renderer, to_string(turnCount).c_str());

    x = turnTallyText->GetPosition().first + turnTallyText->GetWidth() - (turnTallyNumText->GetWidth() * 1.5);

    turnTallyNumText->SetPosition(x, y);
    text.push_back(turnTallyNumText);

    textSize = 65;
    x = turnTallyText->GetPosition().first + 25;
    y = turnTallyText->GetPosition().second + turnTallyText->GetHeight() + 10;

    turnTallyText->SetPosition(turnTallyText->GetPosition().first + (turnTallyNumText->GetWidth() / 2), turnTallyText->GetPosition().second);

    turnTallyText->SetTextContent("Turn", renderer);

    std::string peaksLeftString = "peaks left: " + to_string(unclaimedPeakCount);
    peaksLeftText = new Text("peaksLeftText", "Fonts/yoster.ttf", White, x, y, textSize, renderer, peaksLeftString.c_str());
    text.push_back(peaksLeftText);
    
    currentPlayerCircle = new UIElement("currentPlayerCircle", textures["player circle"][0], surfaces["player circle"], true, false, renderer);
    uiElements.push_back(currentPlayerCircle);
    gameObjects[gameObjects.size() - 1].push_back(currentPlayerCircle);
    currentPlayerCircle->SetGlobalCenter(SCREEN_WIDTH / 2 - 625, SCREEN_HEIGHT - currentPlayerCircle->GetDimensions().second / 2 - (SCREEN_HEIGHT / 40));

    textSize = 125;
    y = 50;

    for (int i = 0; i < rules->GetPlayerCount(); i++)
    {
        x = 50;
        if (i > 0)
        {
            y += players[0]->GetTurnText()->GetDimensions().second;
        }
        Text *playerText = new Text("player" + to_string(i) + "Text", "Fonts/yoster.ttf", players[i]->GetSDLColor(), x, y, textSize, renderer, ("P" + to_string(i + 1) + ":").c_str());
        x = playerText->GetPosition().first + playerText->GetWidth() + 25;
        y = playerText->GetPosition().second;
        Text *playerScoreText = new Text("player" + to_string(i) + "ScoreText", "Fonts/yoster.ttf", White, x, y, textSize, renderer, "0");
        Text *playerCircleText = new Text("player" + to_string(i) + "CircleText", "Fonts/yoster.ttf", White, x, y, textSize, renderer, ("P" + to_string(i + 1)).c_str());
        playerCircleText->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        playerCircleText->SetRenderShadow(false);
        playerCircleText->SetRendered(false);
        text.push_back(playerText);
        text.push_back(playerScoreText);
        text.push_back(playerCircleText);
        players[i]->SetTurnText(playerText);
        players[i]->SetScoreText(playerScoreText);
        players[i]->SetCircleText(playerCircleText);
        if (i == 0)
        {
            playerCircleText->SetRendered(true);
            SDL_SetTextureColorMod(currentPlayerCircle->GetTexture(), currentTurn->GetSDLColor().r / 2, currentTurn->GetSDLColor().g / 2, currentTurn->GetSDLColor().b / 2);
        }
    }

    textSize = 150;

    movesLeftText = new Text("movesLeftText", "Fonts/yoster.ttf", White, 0, 0, textSize, renderer, "11");

    x = SCREEN_WIDTH / 2 - (movesLeftText->GetWidth() / 2);
    y = SCREEN_HEIGHT - movesLeftText->GetHeight() - 225;
    movesLeftText->SetPosition(x, y);

    text.push_back(movesLeftText);

    textSize = 75;

    Text* turnText = new Text("turnText", "Fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Turn");
    turnText->SetPosition((SCREEN_WIDTH / 2) + 500, SCREEN_HEIGHT - turnText->GetHeight() - (SCREEN_HEIGHT / 40));
    text.push_back(turnText);

    Text *endText = new Text("endText", "Fonts/yoster.ttf", White, 0, 0, textSize, renderer, "End");
    endText->SetPosition((SCREEN_WIDTH / 2) + 500 + ((turnText->GetWidth() - endText->GetWidth()) / 2), SCREEN_HEIGHT - turnText->GetHeight() - endText->GetHeight() - (SCREEN_HEIGHT / 40));
    text.push_back(endText);

    UIElement * endTurnArrow = new UIElement("endTurnArrow", textures["end turn arrow"][0], surfaces["end turn arrow"], true, true, renderer);
    uiElements.push_back(endTurnArrow);
    endTurnArrow->SetScale(0.8);
    gameObjects[gameObjects.size() - 1].push_back(endTurnArrow);
    endTurnArrow->SetGlobalCenter(endText->GetPosition().first + endText->GetWidth() / 2, endText->GetPosition().second - 10 - endTurnArrow->GetDimensions().second / 2);
    endTurnArrow->SetRenderShadow(true);
    
}

void loadUI()
{
    UIElement *die1 = new UIElement("dieOne", textures["die 1"][0], surfaces["die 1"], true, true, renderer);
    die1->SetScale(0.15);
    uiElements.push_back(die1);
    gameObjects[gameObjects.size() - 1].push_back(die1);
    die1->SetGlobalCenter(SCREEN_WIDTH / 2 - (die1->GetDimensions().first * die1->GetScale() / 1.25f), SCREEN_HEIGHT - (die1->GetDimensions().second * die1->GetScale() / 2) - (SCREEN_HEIGHT / 40));

    UIElement *die2 = new UIElement("dieTwo", textures["die 2"][0], surfaces["die 2"], true, true, renderer);
    die2->SetScale(0.15);
    uiElements.push_back(die2);
    gameObjects[gameObjects.size() - 1].push_back(die2);
    die2->SetGlobalCenter(SCREEN_WIDTH / 2 + (die2->GetDimensions().first * die2->GetScale() / 1.25f), SCREEN_HEIGHT - (die2->GetDimensions().second * die2->GetScale() / 2) - (SCREEN_HEIGHT / 40));


    for (int i = 0; i < peaks.size(); i++)
    {
        UIElement *claimPeakButton = new UIElement("claim peak button", textures["claim peak"][0], surfaces["claim peak"], false, true, renderer, peaks[i]);
        claimPeakButton->SetScale(0.1);
        claimPeakButton->SetCenter(peaks[i]->GetCenter().first + 50, peaks[i]->GetCenter().second - 50);
        claimPeakButton->SetResizable(true);
        claimPeakButton->SetTopLayer(false);
        uiElements.push_back(claimPeakButton);
        gameObjects[gameObjects.size() - 1].push_back(claimPeakButton);
        peaks[i]->SetClaimNotif(claimPeakButton);
    }
    currentRoll = Roll();
    movesLeft = currentRoll;
}

void GeneratePixels()
{
    bool start = true;
    int totalColumns = MAP_WIDTH + MAP_WIDTH / 2;
    int startPos = 0 - MAP_WIDTH / 4;
    int endPos = startPos + totalColumns;
    for (int i = startPos; i < endPos; i++)
    {
        for (int j = -MAP_HEIGHT / 4; j < MAP_HEIGHT + MAP_HEIGHT / 4; j++)
        {
            int width = 12;
            int height = 12;

            int x = i * width + width / 2;
            int y = j * height + height / 2;
            Terrain *currentTerrain = selectTerrain(x, y, false);
            if (currentTerrain == nullptr)
                continue;

            Pixel *pixel = new Pixel("pixel", textures["pixel"][0], surfaces["pixel"], false, true);

            pixel->SetHiddenTerrain(selectTerrain(x, y, false));
            pixel->SetGlobalPosition(x - width / 2, y - height / 2);
            pixel->SetWidth(width);
            pixel->SetHeight(height);

            SDL_Color pixelColor;
            if (pixel->GetHiddenTerrain()->GetLayer() == 1 && pixel->GetHiddenTerrain()->GetBiome() == "plains")
            {
                Uint8 randR = rand() % 56 + 200;
                Uint8 randG = randR - 8;
                Uint8 randB = randR - 50;
                pixelColor = {randR, randG, randB, 255};
            }
            else
            {
                Uint8 randR = rand() % 11;
                Uint8 randG = rand() % 11;
                Uint8 randB = rand() % 11;
                Uint8 randT = rand() % 2;
                if (pixel->GetHiddenTerrain()->GetBiome() == "plains")
                {
                    pixelColor = {static_cast<Uint8>(125 / rules->GetMaxHeight() * pixel->GetHiddenTerrain()->GetLayer() + (randR * randT)),
                                  static_cast<Uint8>(std::min(125 / rules->GetMaxHeight() * currentTerrain->GetLayer() * 2 + (randG * randT), 255)),
                                  static_cast<Uint8>(125 / rules->GetMaxHeight() * currentTerrain->GetLayer() + (randB * randT)), 255};
                }
                else if (pixel->GetHiddenTerrain()->GetBiome() == "stone")
                {
                    pixelColor = {static_cast<Uint8>(200 / rules->GetMaxHeight() * pixel->GetHiddenTerrain()->GetLayer() + (randR * randT)),
                                  static_cast<Uint8>(200 / rules->GetMaxHeight() * pixel->GetHiddenTerrain()->GetLayer() + (randG * randT)),
                                  static_cast<Uint8>(200 / rules->GetMaxHeight() * currentTerrain->GetLayer() + (randB * randT)), 255};
                }
                else if (pixel->GetHiddenTerrain()->GetBiome() == "glacier")
                {
                    pixelColor = {static_cast<Uint8>(185 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1)),
                                  static_cast<Uint8>(std::min(200 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + 100, 255)),
                                  static_cast<Uint8>(std::min(255 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + 140, 255)), 255};
                }
            }
            pixel->SetColor(pixelColor);
            pixels.push_back(pixel);
        }
    }
}

void GenerateOcean()
{
    int scale = 8;

    for (int i = 0; i < 11; i++)
    {
        ocean.resize(11);
        for (int j = 0; j < 11; j++)
        {
            int index = rand() % 20 + 1;
            OceanTile *oceanTile = new OceanTile("ocean tile edge (" + to_string(i) + ", " + to_string(j) + ")", textures["tile " + to_string(index)][0], surfaces["tile " + to_string(index)], false, true);
            oceanTile->SetGlobalPosition(-3584 + (i * 128 * scale), -4704 + (j * 128 * scale));
            oceanTile->SetScale(scale);
            ocean[i].push_back(oceanTile);
            gameObjects[0].push_back(oceanTile);
        }
    }
}

void renderPixels()
{
    for (int i = 0; i < pixels.size(); i++)
    {
        pixels[i]->RenderGameObject(renderer, hoveringTerrain);
    }
}