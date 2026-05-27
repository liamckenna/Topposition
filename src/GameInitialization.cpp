#include "GameInitialization.h"
#include "Rendering.h"
#include "MouseLogic.h"
#include "AudioManager.h"

void ResetMap()
{
    gameState prevState = state;
    state = LOADING;
    RenderLoadingScreen();
    ClearObjects();
    ResetGlobalVars();
    loadGame();
    if (prevState == PAUSED)
    {
        state = GAME;
    }
    else
    {
        state = prevState;
    }
    SetCameraAtCenter();
    ZoomOutCamera();
}

void loadGame()
{
    Uint64 mapStartTime = SDL_GetTicks();
    loadMap();
    Uint64 mapEndTime = SDL_GetTicks();
    std::cout << "Map loaded in " << (mapEndTime - mapStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 gamePiecesStartTime = SDL_GetTicks();
    loadGamePieces();
    Uint64 gamePiecesEndTime = SDL_GetTicks();
    std::cout << "Game Pieces loaded in " << (gamePiecesEndTime - gamePiecesStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 textStartTime = SDL_GetTicks();
    loadText();
    Uint64 textEndTime = SDL_GetTicks();
    std::cout << "Text loaded in " << (textEndTime - textStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 uiStartTime = SDL_GetTicks();
    loadUI();
    Uint64 uiEndTime = SDL_GetTicks();
    std::cout << "UI loaded in " << (uiEndTime - uiStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 battleSequenceStartTime = SDL_GetTicks();
    loadBattleSequence();
    Uint64 battleSequenceEndTime = SDL_GetTicks();
    std::cout << "Battle Sequence loaded in " << (battleSequenceEndTime - battleSequenceStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 audioStartTime = SDL_GetTicks();
    loadAudio();
    Uint64 audioEndTime = SDL_GetTicks();
    std::cout << "Audio loaded in " << (audioEndTime - audioStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 pixelsStartTime = SDL_GetTicks();
    GeneratePixels();
    Uint64 pixelsEndTime = SDL_GetTicks();
    std::cout << "Pixels Generated in " << (pixelsEndTime - pixelsStartTime) / 1000.f << " seconds!" << std::endl;
    
    Uint64 oceanStartTime = SDL_GetTicks();
    GenerateOcean();
    Uint64 oceanEndTime = SDL_GetTicks();
    std::cout << "Ocean Generated in " << (oceanEndTime - oceanStartTime) / 1000.f << " seconds!" << std::endl;

    Uint64 renderPixelsStartTime = SDL_GetTicks();
    renderPixels();
    SDL_SetRenderTarget(renderer, NULL);
    Uint64 renderPixelsEndTime = SDL_GetTicks();
    std::cout << "Pixels Rendered in " << (renderPixelsEndTime - renderPixelsStartTime) / 1000.f << " seconds!" << std::endl;

    gameStartTime = SDL_GetTicks();
    std::cout << "Total Load Time: " << (gameStartTime - mapStartTime) / 1000.f << " seconds!" << std::endl;
}

void SetCameraAtCenter()
{
    cameraPosition.first = (2560.f) - (worldResolution.first / 2);
    cameraPosition.second = (1440.f) - (worldResolution.second / 2);
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
        bool selectable;
        Player *player = new Player();
        players.push_back(player);
        player->RefreshColorVars();
        switch (j)
        {
        case 0:
            selectable = true;
            break;
        case 1:
            selectable = false;
            break;
        case 2:
            selectable = false;
            break;
        case 3:
            selectable = false;
            break;
        }
        player->SetCircleTextString("P" + std::to_string(j + 1));
        string flagName = player->GetColor() + " flag";
        for (int i = 0; i < peaks.size(); i++)
        {
            Piece *flag = new Piece(player->GetColor(), textures[flagName][0], surfaces[flagName], false);
            flag->SetScale(4);
            flag->SetBottomMiddle(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
            flag->type = FLAG;

            pieces.push_back(flag);
            gameObjects[gameObjects.size() - 2].push_back(flag);
            peaks[i]->flags.push_back(flag);
            player->flags.push_back(flag);
            flag->SetPlayer(player);
        }
        string pieceName = player->GetColor() + " piece float";

        for (int i = 0; i < rules->GetPieces(); i++)
        {
            int x;
            int y;
            int tries = 0;
            int maxTries = 1000;
            do
            {
                x = (rand() % (int)(MAP_WIDTH * 2) / 2);
                y = (rand() % (int)(MAP_HEIGHT * 2) / 2);
                
                switch (j % 4)
                {
                case 0:
                    break;
                case 1:
                    x += MAP_WIDTH;
                    break;
                case 2:
                    x += MAP_WIDTH;
                    y += MAP_HEIGHT;
                    break;
                case 3:
                    y += MAP_HEIGHT;
                    break;
                default:
                    break;
                }
                tries++;
                if (tries > maxTries)
                {
                    switch (j % 4)
                    {
                        case 0:
                            x = (rand() % (int)(MAP_WIDTH * 2) / 2) - (MAP_WIDTH / 2);
                            y = (rand() % (int)(MAP_HEIGHT * 2) / 2) - (MAP_HEIGHT / 2);
                            break;
                        case 1:
                            x = (rand() % (int)(MAP_WIDTH * 2) / 2) + MAP_WIDTH + (MAP_WIDTH / 2);
                            y = (rand() % (int)(MAP_HEIGHT * 2) / 2) - (MAP_HEIGHT / 2);
                            break;
                        case 2:
                            x = (rand() % (int)(MAP_WIDTH * 2) / 2) + MAP_WIDTH  + (MAP_WIDTH / 2);
                            y = (rand() % (int)(MAP_HEIGHT * 2) / 2) + MAP_HEIGHT  + (MAP_HEIGHT / 2);
                            break;
                        case 3:
                            x = (rand() % (int)(MAP_WIDTH * 2) / 2) - (MAP_WIDTH / 2);
                            y = (rand() % (int)(MAP_HEIGHT * 2) / 2) + MAP_HEIGHT  + (MAP_HEIGHT / 2);
                            break;
                        default:
                            x = (rand() % (int)(MAP_WIDTH * 3)) - MAP_WIDTH / 4;
                            y = (rand() % (int)(MAP_HEIGHT * 3)) - MAP_HEIGHT / 4;
                    }
                    break;
                }
            } while (selectTerrain(x, y, false) != NULL || selectUI(x, y, false, false) != NULL);

            Piece *piece = new Piece(pieceName + " " + std::to_string(i), textures["blue piece float"][0], surfaces["blue piece float"], true);
            piece->SetScale(3);
            gameObjects[rules->GetMaxHeight() + 1].push_back(piece);
            piece->SetBottomMiddle(x, y);
            piece->SetDesignatedLocation(x, y);
            piece->SetSelectable(selectable);
            Animation *floatIdle = new Animation(textures[player->GetColor() + " piece float sheet"][0], surfaces[player->GetColor() + " piece float sheet"], 1, 12, {3, 4}, {48, 48});
            Animation *saluteIdle = new Animation(textures[player->GetColor() + " piece salute sheet"][0], surfaces[player->GetColor() + " piece salute sheet"], 2, 24, {4, 6}, {48, 48});
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
    SDL_Color Gold = {239, 191, 4};

    int textSize = 125 * (SCREEN_WIDTH / 3840.f);

    turnTallyText = new Text("turnTallyText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Turn 80");

    int x = SCREEN_WIDTH - turnTallyText->GetWidth() - SCREEN_WIDTH / 80;
    int y = SCREEN_HEIGHT / 40;

    turnTallyText->SetPosition(x, y);
    text.push_back(turnTallyText);

    turnTallyNumText = new Text("turnTallyNumText", "fonts/yoster.ttf", White, x, y, textSize, renderer, to_string(turnCount).c_str());

    x = turnTallyText->GetPosition().first + turnTallyText->GetWidth() - (turnTallyNumText->GetWidth() * 1.5);

    turnTallyNumText->SetPosition(x, y);
    text.push_back(turnTallyNumText);

    textSize = 65 * (SCREEN_WIDTH / 3840.f);
    x = turnTallyText->GetPosition().first + (SCREEN_WIDTH / 160);
    y = turnTallyText->GetPosition().second + turnTallyText->GetHeight() + (SCREEN_HEIGHT / 200);

    lastTurnText = new Text("lastTurnText", "fonts/yoster.ttf", Yellow, x, y, 90 * (SCREEN_WIDTH / 3840.f), renderer, "Last Turn!");
    lastTurnText->SetCenter(turnTallyText->GetCenter().first, turnTallyText->GetCenter().second);
    lastTurnText->SetPosition(lastTurnText->GetPosition().first + (SCREEN_WIDTH / 400), turnTallyText->GetBottomRight().second - lastTurnText->GetHeight());
    lastTurnText->SetGameStateContext(GAME);
    lastTurnText->SetRendered(false);
    text.push_back(lastTurnText);

    suddenDeathText = new Text("suddenDeathText", "fonts/yoster.ttf", Red, x, y, textSize, renderer, "Sudden Death!");
    suddenDeathText->SetSize(70 * (SCREEN_WIDTH / 3840.f), renderer);
    suddenDeathText->SetCenter(turnTallyText->GetCenter().first, turnTallyText->GetCenter().second);
    suddenDeathText->SetPosition(suddenDeathText->GetPosition().first, turnTallyText->GetBottomRight().second - suddenDeathText->GetHeight() - (SCREEN_HEIGHT / 200));
    suddenDeathText->SetGameStateContext(GAME);
    suddenDeathText->SetRendered(false);
    text.push_back(suddenDeathText);

    turnTallyText->SetPosition(turnTallyText->GetPosition().first + (turnTallyNumText->GetWidth() / 2), turnTallyText->GetPosition().second);
    turnTallyText->SetTextContent("Turn", renderer);



    std::string peaksLeftString = "peaks left: 22";
    peaksLeftText = new Text("peaksLeftText", "fonts/yoster.ttf", White, x, y, textSize, renderer, peaksLeftString.c_str());
    text.push_back(peaksLeftText);

    std::string peaksLeftNumString = to_string(22);
    peaksLeftNumText = new Text("peaksLeftNumText", "fonts/yoster.ttf", White, x, y, textSize, renderer, peaksLeftNumString.c_str());
    peaksLeftNumText->SetPosition(peaksLeftText->GetBottomRight().first - peaksLeftNumText->GetWidth(), peaksLeftText->GetPosition().second);
    std::pair<float, float> peaksLeftNumCenter = peaksLeftNumText->GetCenter();
    peaksLeftNumText->SetTextContent(to_string(unclaimedPeakCount).c_str(), renderer);
    peaksLeftNumText->SetCenter(peaksLeftNumCenter.first, peaksLeftNumCenter.second);
    text.push_back(peaksLeftNumText);

    peaksLeftText->SetTextContent("peaks left: ", renderer);

    currentPlayerCircle = new UIElement("currentPlayerCircle", textures["player circle"][0], surfaces["player circle"], true, false, renderer, GAME);
    currentPlayerCircle->SetScale(1.f * (SCREEN_WIDTH / 3840.f));
    uiElements.push_back(currentPlayerCircle);
    gameObjects[gameObjects.size() - 1].push_back(currentPlayerCircle);
    currentPlayerCircle->SetGlobalCenter((SCREEN_WIDTH / 2) - (SCREEN_WIDTH / 7.50f), SCREEN_HEIGHT - (currentPlayerCircle->GetDimensions().second * currentPlayerCircle->GetScale() / 2) - (SCREEN_HEIGHT / 40));
    currentPlayerCircle->SetRenderShadow(true);

    opposingPlayerCircle = new UIElement("opposingPlayerCircle", textures["opposing player circle"][0], surfaces["opposing player circle"], true, false, renderer, GAME);
    opposingPlayerCircle->SetScale(1.f * (SCREEN_WIDTH / 3840.f));
    uiElements.push_back(opposingPlayerCircle);
    gameObjects[gameObjects.size() - 1].push_back(opposingPlayerCircle);
    opposingPlayerCircle->SetGlobalCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), SCREEN_HEIGHT - (opposingPlayerCircle->GetDimensions().second * opposingPlayerCircle->GetScale() / 2) - (SCREEN_HEIGHT / 40));
    opposingPlayerCircle->SetRendered(false);
    opposingPlayerCircle->SetRenderShadow(true);

    textSize = 125 * (SCREEN_WIDTH / 3840.f);
    y = SCREEN_HEIGHT / 40;

    for (int i = 0; i < rules->GetPlayerCount(); i++)
    {
        x = SCREEN_WIDTH / 80;
        if (i > 0)
        {
            y += players[0]->GetTurnText()->GetDimensions().second;
        }
        Text *playerText = new Text("player" + to_string(i) + "Text", "fonts/yoster.ttf", players[i]->GetSDLColor(), x, y, textSize, renderer, ("P" + to_string(i + 1) + ":").c_str());
        x = playerText->GetPosition().first + playerText->GetWidth() + (SCREEN_WIDTH / 160);
        y = playerText->GetPosition().second;
        Text *playerScoreText = new Text("player" + to_string(i) + "ScoreText", "fonts/yoster.ttf", White, x, y, textSize, renderer, "0");
        x += playerScoreText->GetWidth();
        textSize = 75 * (SCREEN_WIDTH / 3840.f);
        Text *playerFirstText = new Text("player" + to_string(i) + "FirstText", "fonts/yoster.ttf", Gold, x, y, textSize, renderer, "1st!");
        playerFirstText->SetPosition(x, y - (playerFirstText->GetHeight() / 4));
        playerFirstText->SetRendered(false);
        Text *playerTieText = new Text("player" + to_string(i) + "TieText", "fonts/yoster.ttf", Gold, x, y, textSize, renderer, "Tie!");
        playerTieText->SetPosition(x, y - (playerTieText->GetHeight() / 4));
        playerTieText->SetRendered(false);
        textSize = 125 * (SCREEN_WIDTH / 3840.f);
        Text *playerCircleText = new Text("player" + to_string(i) + "CircleText", "fonts/yoster.ttf", White, x, y, textSize, renderer, ("P" + to_string(i + 1)).c_str());
        playerCircleText->SetCenter(currentPlayerCircle->GetCenter().first, currentPlayerCircle->GetCenter().second);
        playerCircleText->SetRenderShadow(false);
        playerCircleText->SetRendered(false);
        
        text.push_back(playerText);
        text.push_back(playerScoreText);
        text.push_back(playerFirstText);
        text.push_back(playerTieText);
        text.push_back(playerCircleText);
        players[i]->SetTurnText(playerText);
        players[i]->SetScoreText(playerScoreText);
        players[i]->SetFirstText(playerFirstText);
        players[i]->SetTieText(playerTieText);
        players[i]->SetCircleText(playerCircleText);
        if (i == 0)
        {
            playerCircleText->SetRendered(true);
            SDL_SetTextureColorMod(currentPlayerCircle->GetTexture(), currentTurn->GetSDLColor().r / 1.5f, currentTurn->GetSDLColor().g / 1.5f, currentTurn->GetSDLColor().b / 1.5f);
        }
    }

    textSize = 150 * (SCREEN_WIDTH / 3840.f);

    movesLeftText = new Text("movesLeftText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "11");
    movesLeftText->SetSelectable(true);

    x = (SCREEN_WIDTH / 2) - (movesLeftText->GetWidth() / 2);
    y = SCREEN_HEIGHT - movesLeftText->GetHeight() - (SCREEN_HEIGHT / 9);
    movesLeftText->SetPosition(x, y);

    text.push_back(movesLeftText);

    textSize = 75 * (SCREEN_WIDTH / 3840.f);

    turnText = new Text("turnText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Turn");
    turnText->SetPosition((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 8), SCREEN_HEIGHT - turnText->GetHeight() - (SCREEN_HEIGHT / 40));
    turnText->SetCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), turnText->GetCenter().second);
    turnText->SetSelectable(true);
    text.push_back(turnText);

    endText = new Text("endText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "End");
    endText->SetPosition((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 8) + ((turnText->GetWidth() - endText->GetWidth()) / 2), SCREEN_HEIGHT - turnText->GetHeight() - endText->GetHeight() - (SCREEN_HEIGHT / 40));
    endText->SetCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), endText->GetCenter().second);
    endText->SetSelectable(true);
    text.push_back(endText);

    endTurnArrow = new UIElement("endTurnArrow", textures["end turn arrow"][0], surfaces["end turn arrow"], true, true, renderer, GAME);
    uiElements.push_back(endTurnArrow);
    endTurnArrow->SetScale(0.8f * (SCREEN_WIDTH / 3840.f));
    gameObjects[gameObjects.size() - 1].push_back(endTurnArrow);
    endTurnArrow->SetGlobalCenter((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 7.50f), endText->GetPosition().second - (SCREEN_HEIGHT / 200) - (endTurnArrow->GetDimensions().second * endTurnArrow->GetScale() / 3));
    endTurnArrow->SetRenderShadow(true);

    textSize = 200 * (SCREEN_WIDTH / 3840.f);
    pausedText = new Text("pausedText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Paused");
    pausedText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10);
    pausedText->SetRenderShadow(false);
    pausedText->SetGameStateContext(PAUSED);
    pausedText->SetPauseStateContext(MAIN);
    text.push_back(pausedText);

    textSize = 150 * (SCREEN_WIDTH / 3840.f);
    resetMapText = new Text("resetMapText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Reset");
    resetMapText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8);
    resetMapText->SetRenderShadow(false);
    resetMapText->SetGameStateContext(PAUSED);
    resetMapText->SetPauseStateContext(MAIN);
    resetMapText->SetSelectable(true);
    text.push_back(resetMapText);

    mainExitToMainMenuText = new Text("mainExitToMainMenuText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Main Menu");
    mainExitToMainMenuText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8 + SCREEN_HEIGHT / 10);
    mainExitToMainMenuText->SetRenderShadow(false);
    mainExitToMainMenuText->SetGameStateContext(PAUSED);
    mainExitToMainMenuText->SetPauseStateContext(MAIN);
    mainExitToMainMenuText->SetSelectable(true);
    text.push_back(mainExitToMainMenuText);

    textSize = 200 * (SCREEN_WIDTH / 3840.f);
    playerWinsText = new Text("playerWinsText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Player 1 Wins!");
    playerWinsText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10);
    playerWinsText->SetRenderShadow(false);
    playerWinsText->SetGameStateContext(PAUSED);
    playerWinsText->SetPauseStateContext(WINNER);
    text.push_back(playerWinsText);

    textSize = 150 * (SCREEN_WIDTH / 3840.f);
    playAgainText = new Text("playAgainText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Play Again");
    playAgainText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8);
    playAgainText->SetRenderShadow(false);
    playAgainText->SetGameStateContext(PAUSED);
    playAgainText->SetPauseStateContext(WINNER);
    playAgainText->SetSelectable(true);
    text.push_back(playAgainText);

    winnerExitToMainMenuText = new Text("winnerExitToMainMenuText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "Main Menu");
    winnerExitToMainMenuText->SetCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 8 + SCREEN_HEIGHT / 10);
    winnerExitToMainMenuText->SetRenderShadow(false);
    winnerExitToMainMenuText->SetGameStateContext(PAUSED);
    winnerExitToMainMenuText->SetPauseStateContext(WINNER);
    winnerExitToMainMenuText->SetSelectable(true);
    text.push_back(winnerExitToMainMenuText);

    moveCostText = new Text("moveCostText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "-0");
    moveCostText->SetSize(65 * (SCREEN_WIDTH / 3840.f), renderer);
    moveCostText->SetCenter(movesLeftText->GetBottomRight().first + (SCREEN_WIDTH / 200), movesLeftText->GetPosition().second + (moveCostText->GetHeight() / 2));
    moveCostText->SetGameStateContext(GAME);
    text.push_back(moveCostText);

    fpsCounterText = new Text("fpsCounterText", "fonts/yoster.ttf", White, 0, 0, textSize, renderer, "1444 FPS");
    fpsCounterText->SetSize(40 * (SCREEN_WIDTH / 3840.f), renderer);
    fpsCounterText->SetPosition(players[0]->GetTurnText()->GetPosition().first, turnTallyText->GetPosition().second - fpsCounterText->GetHeight());
    fpsCounterText->SetShadowOffset(5, 5);
    fpsCounterText->SetGameStateContext(GAME);
    text.push_back(fpsCounterText);
}

void loadUI()
{
    die1 = new UIElement("dieOne", textures["die 1"][0], surfaces["die 1"], true, true, renderer, GAME);
    die1->SetScale(0.15 * (SCREEN_WIDTH / 3840.f));
    uiElements.push_back(die1);
    gameObjects[gameObjects.size() - 1].push_back(die1);
    die1->SetGlobalCenter((SCREEN_WIDTH / 2) - (die1->GetDimensions().first * die1->GetScale() / 1.25f), SCREEN_HEIGHT - (die1->GetDimensions().second * die1->GetScale() / 2) - (SCREEN_HEIGHT / 40));
    die1->SetRenderShadow(true);


    die2 = new UIElement("dieTwo", textures["die 2"][0], surfaces["die 2"], true, true, renderer, GAME);
    die2->SetScale(0.15 * (SCREEN_WIDTH / 3840.f));
    uiElements.push_back(die2);
    gameObjects[gameObjects.size() - 1].push_back(die2);
    die2->SetGlobalCenter((SCREEN_WIDTH / 2) + (die2->GetDimensions().first * die2->GetScale() / 1.25f), SCREEN_HEIGHT - (die2->GetDimensions().second * die2->GetScale() / 2) - (SCREEN_HEIGHT / 40));
    die2->SetRenderShadow(true);

    crown = new UIElement("crown", textures["crown"][0], surfaces["crown"], true, false, renderer, GAME);
    uiElements.push_back(crown);
    gameObjects[gameObjects.size() - 1].push_back(crown);
    crown->SetScale(8 * (SCREEN_WIDTH / 3840.f));
    crown->SetGlobalPosition(die1->GetGlobalCenter().first - (die1->GetDimensions().first * die1->GetScale() / 2) - (crown->GetDimensions().first * crown->GetScale()), die1->GetGlobalCenter().second - (die1->GetDimensions().second * die1->GetScale() / 2) - (crown->GetDimensions().second * crown->GetScale()));
    crown->SetRenderShadow(true);
    crown->SetRendered(false);

    fatalAttackExclamation = new UIElement("fatalAttackExclamation", textures["exclamation point"][0], surfaces["exclamation point"], true, false, renderer, GAME);
    fatalAttackExclamation->SetScale(8 * (SCREEN_WIDTH / 3840.f));
    fatalAttackExclamation->SetGlobalPosition(die2->GetGlobalCenter().first + (die2->GetDimensions().first * die2->GetScale() / 2) + (fatalAttackExclamation->GetDimensions().first * fatalAttackExclamation->GetScale()), die2->GetGlobalCenter().second - (die2->GetDimensions().second * die2->GetScale() / 2) - (fatalAttackExclamation->GetDimensions().second * fatalAttackExclamation->GetScale() / 2));
    fatalAttackExclamation->SetRenderShadow(true);
    fatalAttackExclamation->SetRendered(false);
    uiElements.push_back(fatalAttackExclamation);
    gameObjects[gameObjects.size() - 1].push_back(fatalAttackExclamation);

    fatalDefenseExclamation = new UIElement("fatalDefenseExclamation", textures["exclamation point"][0], surfaces["exclamation point"], true, false, renderer, GAME);
    fatalDefenseExclamation->SetScale(8 * (SCREEN_WIDTH / 3840.f));
    fatalDefenseExclamation->SetGlobalPosition(die1->GetGlobalCenter().first - (die1->GetDimensions().first * die1->GetScale() / 2) - (fatalDefenseExclamation->GetDimensions().first * fatalDefenseExclamation->GetScale()) - (fatalDefenseExclamation->GetDimensions().first * fatalDefenseExclamation->GetScale()), die1->GetGlobalCenter().second - (die1->GetDimensions().second * die1->GetScale() / 2) - (fatalDefenseExclamation->GetDimensions().second * fatalDefenseExclamation->GetScale() / 2));
    fatalDefenseExclamation->SetRenderShadow(true);
    fatalDefenseExclamation->SetRendered(false);
    uiElements.push_back(fatalDefenseExclamation);
    gameObjects[gameObjects.size() - 1].push_back(fatalDefenseExclamation);

    for (int i = 0; i < peaks.size(); i++)
    {
        UIElement *claimPeakButton = new UIElement("claim peak button", textures["claim peak"][0], surfaces["claim peak"], false, true, renderer, GAME, peaks[i]);
        claimPeakButton->SetScale(4);
        claimPeakButton->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
        claimPeakButton->SetResizable(true);
        claimPeakButton->SetTopLayer(false);
        uiElements.push_back(claimPeakButton);
        gameObjects[gameObjects.size() - 1].push_back(claimPeakButton);
        peaks[i]->SetClaimNotif(claimPeakButton);
        UIElement *defendPeakButton = new UIElement("defend peak button", textures["defend peak"][0], surfaces["defend peak"], false, true, renderer, GAME, peaks[i]);
        defendPeakButton->SetScale(4);
        defendPeakButton->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
        defendPeakButton->SetResizable(true);
        defendPeakButton->SetTopLayer(false);
        uiElements.push_back(defendPeakButton);
        gameObjects[gameObjects.size() - 1].push_back(defendPeakButton);
        peaks[i]->SetDefendNotif(defendPeakButton);
    }
    if (rules->GetAutoRoll())
    {
        currentRoll = Roll();
        movesLeft = currentRoll;
    }
    else
    {
        ClearRoll();
    }
    for (int i = 0; i < players.size(); i++)
    {
        for(int j = 0; j < players[i]->soldiers.size(); j++)
        {
            UIElement *soldierHead = new UIElement(players[i]->GetColor() + " soldier " + std::to_string(j) + " head", textures[players[i]->GetColor() + " player head"][0], surfaces[players[i]->GetColor() + " player head"], true, false, renderer, GAME);
            UIElement *soldierHeadCross = new UIElement(players[i]->GetColor() + " soldier " + std::to_string(j) + " head cross", textures["soldier head cross"][0], surfaces["soldier head cross"], true, false, renderer, GAME);

            soldierHead->SetScale(2.f * (SCREEN_WIDTH / 3840.f));
            soldierHeadCross->SetScale(2.f * (SCREEN_WIDTH / 3840.f));
            soldierHead->SetPosition(peaksLeftText->GetPosition().first + (j * (soldierHead->GetDimensions().first * soldierHead->GetScale() + SCREEN_WIDTH / 600)) - SCREEN_WIDTH / 350, peaksLeftText->GetPosition().second + peaksLeftText->GetHeight() + SCREEN_HEIGHT / 100);
            soldierHeadCross->SetPosition(peaksLeftText->GetPosition().first + (j * (soldierHeadCross->GetDimensions().first * soldierHeadCross->GetScale() + SCREEN_WIDTH / 600)) - SCREEN_WIDTH / 350, peaksLeftText->GetPosition().second + peaksLeftText->GetHeight() + SCREEN_HEIGHT / 100);
            if (i > 0)
            {
                soldierHead->SetRendered(false);
            } 
            soldierHeadCross->SetRendered(false);
            players[i]->soldierHeads.push_back(soldierHead);
            players[i]->soldierHeadCrosses.push_back(soldierHeadCross);
            uiElements.push_back(soldierHead);
            uiElements.push_back(soldierHeadCross);
        }
    }
    RefreshShadows();
}

void loadBattleSequence()
{
    battleSequence = new BattleSequenceState();
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
            if (pixel->GetHiddenTerrain()->GetLayer() == 1 && (pixel->GetHiddenTerrain()->GetBiome() == "plains" || pixel->GetHiddenTerrain()->GetBiome() == "glacier"))
            {
                if (pixel->GetHiddenTerrain()->GetBiome() == "plains")
                {
                    Uint8 randR = rand() % 56 + 200;
                    Uint8 randG = randR - 8;
                    Uint8 randB = randR - 50;
                    pixelColor = {randR, randG, randB, 255};
                }
                else
                {
                    pixelColor = {235, 235, 255, 255};
                }
            }
            else if (pixel->GetHiddenTerrain()->GetBiome() == "glacier" && pixel->GetHiddenTerrain()->GetLayer() == 2)
            {
                Uint8 randR = rand() % 11;
                Uint8 randG = rand() % 11;
                Uint8 randB = rand() % 11;
                Uint8 randT = rand() % 2;
                int randI = (randR + randG + randB) / 3;
                pixelColor = {static_cast<Uint8>(std::min(138 + (randI * randT), 255)),
                              static_cast<Uint8>(std::min(200 + (randI * randT), 255)),
                              static_cast<Uint8>(std::min(255, 255)), 255};
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
                    if (pixel->GetHiddenTerrain() == pixel->GetHiddenTerrain()->GetPeak())
                    {
                        int randI = (randR + randG + randB) / 6;
                        pixelColor = {static_cast<Uint8>(std::min(200 + (randI * randT), 255)),
                                      static_cast<Uint8>(std::min(235 + (randI * randT), 255)),
                                      static_cast<Uint8>(std::min(255, 255)), 255};
                    }
                    else
                    {
                        int randI = (randR + randG + randB) / 6;
                        pixelColor = {static_cast<Uint8>(std::min(185 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + (randI * randT), 255)),
                                      static_cast<Uint8>(std::min(200 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + 100 + (randI * randT), 255)),
                                      static_cast<Uint8>(std::min(255 / rules->GetMaxHeight() * (pixel->GetHiddenTerrain()->GetLayer() - 1) + 140 + (randI * randT), 255)), 255};
                    }
                }
            }
            pixel->SetColor(pixelColor);
            pixels.push_back(pixel);
        }
    }
}

void loadAudio()
{
    AudioManager::loadSound("up", "audio/up.wav");
    AudioManager::loadSound("ehh", "audio/ehh.wav");
    AudioManager::loadSound("hold_up", "audio/hold up.wav");
    AudioManager::loadSound("click", "audio/click.wav");
    AudioManager::loadSound("drop", "audio/drop.wav");
    AudioManager::loadSound("splash", "audio/splash.wav");
    AudioManager::loadSound("roll-crash", "audio/roll-crash.wav");
    AudioManager::loadSound("roll-left", "audio/roll-left.wav");
    AudioManager::loadSound("roll-right", "audio/roll-right.wav");
    AudioManager::loadSound("crash", "audio/crash.wav");
    AudioManager::loadSound("error", "audio/error.wav");
    AudioManager::loadSound("whistle", "audio/whistle.wav");
    AudioManager::loadSound("foghorn", "audio/foghorn.wav");
    AudioManager::loadSound("death", "audio/death.wav");
    AudioManager::loadSound("grunt-left", "audio/grunt-left.wav");
    AudioManager::loadSound("grunt-right", "audio/grunt-right.wav");
    AudioManager::loadSound("lol", "audio/lol.wav");
    AudioManager::loadSound("horn", "audio/horn.wav");
    AudioManager::loadSound("bell", "audio/bell.wav");
    AudioManager::loadSound("victory-theme", "audio/victory-theme.wav");

    AudioManager::setMasterVolume(0.2f);
    AudioManager::loadSound("ocean_ambiance", "audio/ocean-ambiance.wav");
    AudioManager::playMusic("ocean_ambiance", 0.35f);
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

void ClearObjects()
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

    delete battleSequence;
}

void ResetGlobalVars()
{
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
    cameraZoom = 1.f;
    cameraPosition = {0, 0};
    worldResolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
    turnCount = 1;
    turnTallyText = nullptr;
    turnTallyNumText = nullptr;
    peaksLeftText = nullptr;
    peaksLeftNumText = nullptr;
    movesLeftText = nullptr;
    pausedText = nullptr;
    resetMapText = nullptr;
    mainExitToMainMenuText = nullptr;
    turnText = nullptr;
    endText = nullptr;
    suddenDeathText = nullptr;
    lastTurnText = nullptr;
    moveCostText = nullptr;
    fpsCounterText = nullptr;
    currentPlayerCircle = nullptr;
    opposingPlayerCircle = nullptr;
    endTurnArrow = nullptr;
    crown = nullptr;
    die1 = nullptr;
    die2 = nullptr;
    currentTurn = nullptr;
    firstPlace = nullptr;
    fatalAttackExclamation = nullptr;
    fatalDefenseExclamation = nullptr;
    battleSequence = nullptr;
    pState = MAIN;
    hasRolled = false;
    allPeaksClaimed = false;
    suddenDeath = false;
    lastTurn = false;
}

void RefreshShadows()
{
    for (int i = 0; i < uiElements.size(); i++)
    {
        uiElements[i]->RefreshShadow(renderer);
    }

}