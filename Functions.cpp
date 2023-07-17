//
// Created by lmckenna on 6/26/2023.
//

#include "Functions.h"


SDL_Window* window = NULL;

SDL_Surface* screenSurface = NULL;

SDL_Renderer* renderer = NULL;

SDL_Texture* texture = NULL;

int SCREEN_WIDTH = 1920;

int SCREEN_HEIGHT = 1080;

bool quit = false;

int currentRoll = 3;

int movesLeft = 3;


gameState currentState = MAIN_MENU;

std::map<std::string, std::map<int, SDL_Texture*>> textures;

std::map<std::string, SDL_Surface*> surfaces;

std::vector<std::vector<GameObject*>> gameObjects;

std::vector<Peak*> peaks;

std::vector<std::vector<Terrain*>> terrain;

std::vector<Pixel*> pixels;

std::vector<std::vector<GameObject*>> terrainOutlines;

std::vector<UIElement*> uiElements;

std::vector<Piece*> pieces;

std::vector<Text*> text;

std::vector<Animation*> animations;

GameObject* selectedObject = nullptr;

Item* selectedItem = nullptr;

GameRules* rules = new GameRules();

int shapeCount = 0;

string currentTurn = "playerOne";

int playerOneScore = 0;

int playerTwoScore = 0;

int playerThreeScore = 0;

int playerFourScore = 0;

std::vector<Item*> playerOneInventory;

std::vector<Item*> playerTwoInventory;

std::vector<Item*> playerThreeInventory;

std::vector<Item*> playerFourInventory;

std::vector<Item*> currentInventory = playerOneInventory;

Text* playerOneText;

Text* playerTwoText;

Text* playerThreeText;

Text* playerFourText;

Terrain* hoveringTerrain;

bool init()
{
    //Initialization flag
    bool success = true;
    srand((unsigned) time(NULL));


    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }



        //Create window
        window = SDL_CreateWindow( "Topposition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            /*Set Fullscreen
            if (SDL_SetWindowFullscreen(gWindow,SDL_WINDOW_FULLSCREEN_DESKTOP) < 0) {
                printf( "Warning: Fullscreen Failed! SDL Error: %s\n", SDL_GetError() );
            }*/
            //Create renderer for window
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
            if( renderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;

                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
                else {
                    screenSurface = SDL_GetWindowSurface( window );
                    TextureLoader();
                    TTF_Init();
                }
            }
        }
    }

    return success;
}

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

        string flagName = playerNumber + " flag";
        for (int i = 0; i < peaks.size(); i++) {
            Piece* flag = new Piece(playerNumber, textures[flagName][0], surfaces[flagName], false);
            flag->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second - 20);
            pieces.push_back(flag);
            gameObjects[gameObjects.size() - 2].push_back(flag);
            peaks[i]->flags.push_back(flag);

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
        }
    }

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

    SDL_Color White = {255, 255, 255};

    Text* currentTurnText = new Text("currentTurnText", "Fonts/yoster.ttf", White, 150, 0, 300, 50, 100, renderer, "Current Turn: ");
    text.push_back(currentTurnText);
    SDL_Color Red = {255, 0, 0};

    playerOneText = new Text("playerOneText", "Fonts/yoster.ttf", Red, 450, 0, 250, 50, 100, renderer, "Player One");
    text.push_back(playerOneText);

    SDL_Color Green = {0, 255, 0};

    playerTwoText = new Text("playerTwoText", "Fonts/yoster.ttf", Green, 450, 0, 250, 50, 100, renderer, "Player Two");
    text.push_back(playerTwoText);
    playerTwoText->SetRendered(false);

    SDL_Color Blue = {0, 0, 255};

    playerThreeText = new Text("playerThreeText", "Fonts/yoster.ttf", Blue, 450, 0, 300, 50, 100, renderer, "Player Three");
    text.push_back(playerThreeText);
    playerThreeText->SetRendered(false);

    SDL_Color Yellow = {255, 255, 0};

    playerFourText = new Text("playerFourText", "Fonts/yoster.ttf", Yellow, 450, 0, 275, 50, 100, renderer, "Player Four");
    text.push_back(playerFourText);
    playerFourText->SetRendered(false);


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

    for (int i = 0; i < playerOneInventory.size(); i++) {
        playerOneInventory[i] = nullptr;
    }

    for (int i = 0; i < playerTwoInventory.size(); i++) {
        playerTwoInventory[i] = nullptr;
    }

    for (int i = 0; i < playerThreeInventory.size(); i++) {
        playerThreeInventory[i] = nullptr;
    }

    for (int i = 0; i < playerFourInventory.size(); i++) {
        playerFourInventory[i] = nullptr;
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
    std::vector<Item*> newPlayerOneInventory;
    std::vector<Item*> newPlayerTwoInventory;
    std::vector<Item*> newPlayerThreeInventory;
    std::vector<Item*> newPlayerFourInventory;
    std::vector<Pixel*> newPixels;
    std::vector<Text*> newText;
    std::vector<Animation*> newAnimations;
    animations = newAnimations;
    text = newText;
    pixels = newPixels;
    uiElements = newUIElements;
    pieces = newPieces;
    terrain = newTerrain;
    terrainOutlines = newTerrainOutlines;
    gameObjects = newGameObjects;
    peaks = newPeaks;
    playerOneInventory = newPlayerOneInventory;
    playerTwoInventory = newPlayerTwoInventory;
    playerThreeInventory = newPlayerThreeInventory;
    playerFourInventory = newPlayerFourInventory;
    loadMap();
    loadUI();
    loadGamePieces();
    GeneratePixels();

    currentTurn = "playerOne";
    currentInventory = playerOneInventory;

    renderPixels();
    SDL_SetRenderTarget(renderer, NULL);
}

void close()
{
    //Free loaded image
    /*for (auto i : textures) {
        SDL_DestroyTexture(i.second);
        i.second = NULL;
    }*/
    //Destroy window
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    window = NULL;
    renderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface( std::string path )
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, screenSurface->format, 0 );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);

    }
    return optimizedSurface;
}

SDL_Texture* loadTexture( std::string path )
{
    //The final texture
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

void TextureLoader() {
    //load all textures
    struct dirent *entry = nullptr;
    struct dirent* innerEntry = nullptr;
    struct dirent* innerx2 = nullptr;
    DIR *odp = nullptr;
    DIR* idp = nullptr;
    DIR* iidp = nullptr;
    const char* innerDir = "";
    const char* innerx2Dir = "";
    odp = opendir("Textures");
    int textureCount = 0;
    if (odp != nullptr) {
        while (entry = readdir(odp)) {
            if (entry->d_namlen == 1 || entry->d_namlen == 2) continue;
            string fp = "Textures/" + (string)entry->d_name + "/";
            innerDir = fp.c_str();
            idp = opendir(innerDir);
            while (innerEntry = readdir(idp)) {
                if (innerEntry->d_namlen == 1 || innerEntry->d_namlen == 2) continue;
                string fileName = innerEntry->d_name;
                size_t pos = fileName.find(".");
                if (pos == std::string::npos) {
                    string fpp = fp + (string)innerEntry->d_name + "/";
                    innerx2Dir = fpp.c_str();
                    iidp = opendir(innerx2Dir);
                    while (innerx2 = readdir(iidp)) {
                        if (innerx2->d_namlen == 1 || innerx2->d_namlen == 2) continue;
                        string innerFileName = innerx2->d_name;
                        size_t innerPos = innerFileName.find(".");
                        string innerShorthand = innerFileName.substr(0, innerPos);
                        string innerTexturePath = fpp + innerFileName;
                        textures[innerShorthand][0] = loadTexture(innerTexturePath);
                        surfaces[innerShorthand] = loadSurface(innerTexturePath);
                    }
                }
                string shorthand = fileName.substr(0,pos);
                string texturePath = fp + fileName;
                if ((string)entry->d_name == "Terrain") {
                    for (int j = 0; j < rules->GetMaxHeight()+1; j++) {
                        textures[shorthand][j] = loadTexture(texturePath);
                    }
                    shapeCount++;
                } else textures[shorthand][0] = loadTexture(texturePath);
                surfaces[shorthand] = loadSurface(texturePath);
                textureCount++;

            }
            closedir(idp);
        }

    }

    closedir(odp);
    printf("There are %d unique terrain shapes.\n", shapeCount);
    printf("There's %d files in the current directory.\n", textureCount);
}

void RenderScreen(){
    SDL_SetRenderDrawColor( renderer, 51, 169, 255, 100 );
    SDL_RenderClear( renderer );
    //Render texture to screen
    //refreshClaimNotifs();
    //gameObjects[0][0]->RenderGameObject(renderer);
    renderTerrain();
    //renderPixels();
    renderClaimNotifs();
    renderPieces();
    renderUI();
    renderText();
    renderInventory();


    //Update screen
    SDL_RenderPresent( renderer);

}

void zoom(SDL_Event e, Input* playerInput) {

    std::pair<float, float> relativePositionB4;
    std::pair<float, float> newRelativePosition;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetResizable()) {
                if ((e.wheel.preciseY < 0 && gameObjects[i][j]->GetSize() <= 1) || (e.wheel.preciseY > 0 && gameObjects[i][j]->GetSize() >= 3.5)) {
                    continue;
                } else {

                    relativePositionB4.first = (gameObjects[i][j]->GetPosition().first - playerInput->currentMousePosition.first) / gameObjects[i][j]->GetSize();
                    relativePositionB4.second = (gameObjects[i][j]->GetPosition().second - playerInput->currentMousePosition.second) / gameObjects[i][j]->GetSize();
                    gameObjects[i][j]->AdjustSize((e.wheel.preciseY/abs(e.wheel.preciseY) * .1));
                    newRelativePosition.first = (gameObjects[i][j]->GetPosition().first - playerInput->currentMousePosition.first) / gameObjects[i][j]->GetSize();
                    newRelativePosition.second = (gameObjects[i][j]->GetPosition().second - playerInput->currentMousePosition.second) / gameObjects[i][j]->GetSize();

                    gameObjects[i][j]->SetPosition((gameObjects[i][j]->GetPosition().first + (relativePositionB4.first - newRelativePosition.first) * gameObjects[i][j]->GetSize()),
                                                   (gameObjects[i][j]->GetPosition().second + (relativePositionB4.second - newRelativePosition.second) * gameObjects[i][j]->GetSize()));
                    if (gameObjects[i][j]->GetSize() == 1 && i > 0) {
                        gameObjects[i][j]->SetDefaultPosition(gameObjects[i][j]->GetPosition().first, gameObjects[i][j]->GetPosition().second);
                    }
                }

            }

        }
    }
    RecenterScreen(playerInput);
}

void renderObjects(SDL_Renderer* renderer) {
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetRendered()) {
                gameObjects[i][j]->RenderGameObject(renderer);
            }
        }
    }
}

void renderUI() {
    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetAssociatedPeak() == nullptr) {
            uiElements[i]->RenderGameObject(renderer);
        }
    }
}

void renderClaimNotifs() {
    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetAssociatedPeak() != nullptr) {
            uiElements[i]->RenderGameObject(renderer);
        }
    }
}

void renderTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->RenderGameObject(renderer, hoveringTerrain);
        }
    }
}

void renderPieces() {
    for (int i = pieces.size() - 1; i >= 0; i--) {
        if (pieces[i]->type != GameObject::ITEM) {
            pieces[i]->RenderGameObject(renderer);
        }
    }
}

void renderInventory(){
    for (int i = 0; i < currentInventory.size(); i++) {
        currentInventory[i]->RenderGameObject(renderer);
    }
}

void renderPixels(){
    for (int i = 0; i < pixels.size(); i++) {
        pixels[i]->RenderGameObject(renderer, hoveringTerrain);
    }
}

void renderText(){
    for(int i = 0; i < text.size(); i++) {
        text[i]->RenderText(renderer);
    }
}

void scroll(Input* playerInput) {
    bool enabled = true;
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (i == 0) {
                if (gameObjects[i][j]->GetSize() <= 1 ||
                    (gameObjects[i][j]->GetPosition().first > 0 && ((playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) > 0)) ||
                    ((gameObjects[i][j]->GetPosition().first + (gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetDimensions().first)) < SCREEN_WIDTH && ((playerInput->currentMousePosition.first - playerInput->prevMousePosition.first) < 0)) ||
                    (gameObjects[i][j]->GetPosition().second > 0 && ((playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) > 0)) ||
                    ((gameObjects[i][j]->GetPosition().second + (gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetDimensions().second)) < SCREEN_HEIGHT && ((playerInput->currentMousePosition.second - playerInput->prevMousePosition.second) < 0))) {
                    enabled = false;
                    continue;
                } else {
                    enabled = true;
                }
            }
            if (enabled && gameObjects[i][j]->GetResizable()){
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                               gameObjects[i][j]->GetPosition().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
            }
        }
    }

    RecenterScreen(playerInput);
}

GameObject* selectObject(int x, int y) {

    for (int i = gameObjects.size() - 1; i >= 0; i--) {
        for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
            int width_LowerBound = gameObjects[i][j]->GetPosition().first;
            int width_UpperBound = gameObjects[i][j]->GetBottomRight().first;
            int height_LowerBound = gameObjects[i][j]->GetPosition().second;
            int height_UpperBound = gameObjects[i][j]->GetBottomRight().second;

            if (x >= width_LowerBound && x <= width_UpperBound) {
                if (y >= height_LowerBound && y <= height_UpperBound) {

                    SDL_Color color = GetPixelColor(gameObjects[i][j]->GetSurface(),
                                                    (x - width_LowerBound)/(gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale()),
                                                    (y - height_LowerBound)/(gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    }
                    return gameObjects[i][j];

                }
            }

        }
    }
    return nullptr;
}

UIElement* selectUI(int x, int y) {
    for (int i = 0; i < uiElements.size(); i++) {
        int width_LowerBound = uiElements[i]->GetPosition().first;
        int width_UpperBound = uiElements[i]->GetBottomRight().first;
        int height_LowerBound = uiElements[i]->GetPosition().second;
        int height_UpperBound = uiElements[i]->GetBottomRight().second;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(uiElements[i]->GetSurface(),
                                                (x - width_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()),
                                                (y - height_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    continue;
                } else if (uiElements[i]->GetRendered()){
                    return uiElements[i];
                }


            }
        }

    }
    return nullptr;
}

Piece* selectPiece(int x, int y) {
    for (int i = pieces.size() - 1; i >= 0; i--) {
        if (!pieces[i]->GetSelectable() || !pieces[i]->GetRendered()) continue;
        int width_LowerBound = pieces[i]->GetPosition().first;
        int width_UpperBound = pieces[i]->GetBottomRight().first;
        int height_LowerBound = pieces[i]->GetPosition().second;
        int height_UpperBound = pieces[i]->GetBottomRight().second;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(pieces[i]->GetSurface(),
                                                (x - width_LowerBound)/(pieces[i]->GetSize() * pieces[i]->GetScale()),
                                                (y - height_LowerBound)/(pieces[i]->GetSize() * pieces[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    continue;
                } else {
                    Piece* temp = pieces[0];
                    pieces[0] = pieces[i];
                    pieces[i] = temp;
                    return pieces[0];
                }


            }
        }

    }
    return nullptr;
}

Terrain* selectTerrain(int x, int y) {
    for (int i = terrain.size() - 1; i >= 0; i--) {
        for (int j = terrain[i].size() - 1; j >= 0; j--) {
            int width_LowerBound = terrain[i][j]->GetPosition().first;
            int width_UpperBound = terrain[i][j]->GetBottomRight().first;
            int height_LowerBound = terrain[i][j]->GetPosition().second;
            int height_UpperBound = terrain[i][j]->GetBottomRight().second;

            if (x >= width_LowerBound && x <= width_UpperBound) {
                if (y >= height_LowerBound && y <= height_UpperBound) {

                    SDL_Color color = GetPixelColor(terrain[i][j]->GetSurface(),
                                                    (x - width_LowerBound)/(terrain[i][j]->GetSize() * terrain[i][j]->GetScale()),
                                                    (y - height_LowerBound)/(terrain[i][j]->GetSize() * terrain[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    }
                    return terrain[i][j];

                }
            }

        }
    }
    return nullptr;
}

Item* selectItem(int x, int y) {
    for (int i = 0; i < currentInventory.size(); i++) {
        int width_LowerBound = currentInventory[i]->GetPosition().first;
        int width_UpperBound = currentInventory[i]->GetBottomRight().first;
        int height_LowerBound = currentInventory[i]->GetPosition().second;
        int height_UpperBound = currentInventory[i]->GetBottomRight().second;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(currentInventory[i]->GetSurface(),
                                                (x - width_LowerBound)/(currentInventory[i]->GetSize() * currentInventory[i]->GetScale()),
                                                (y - height_LowerBound)/(currentInventory[i]->GetSize() * currentInventory[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    continue;
                } else {
                    return currentInventory[i];
                }


            }
        }

    }
    return nullptr;
}


SDL_Color GetPixelColor(const SDL_Surface * surface, const int X, const int Y)
{
    // Bytes per pixel
    const Uint8 Bpp = surface->format->BytesPerPixel;

    /*
    Retrieve the address to a specific pixel
    pSurface->pixels	= an array containing the SDL_Surface' pixels
    pSurface->pitch		= the length of a row of pixels (in bytes)
    X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
    */
    Uint8* pPixel = (Uint8*)surface->pixels + Y * surface->pitch + X * Bpp;

    Uint32 PixelData = *(Uint32*)pPixel;

    SDL_Color Color = {0x00, 0x00, 0x00};

    // Retrieve the RGB values of the specific pixel
    SDL_GetRGBA(PixelData, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);


    return Color;
}


void moveSelectedObject(GameObject* gameObject, Input* playerInput) {
    if (gameObject->GetMovable()) {
        gameObject->SetCenter(gameObject->GetCenter().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                gameObject->GetCenter().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
        hoveringTerrain = selectTerrain(gameObject->GetCenter().first, gameObject->GetCenter().second + gameObject->GetDimensions().second/2 * gameObject->GetSize() * gameObject->GetScale());
    }
}

void RecenterScreen(Input* playerInput) {
    if (gameObjects[0][0]->GetPosition().first > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first - gameObjects[0][0]->GetPosition().first,
                                                   gameObjects[i][j]->GetPosition().second);
                }
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize() < SCREEN_WIDTH) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + (SCREEN_WIDTH - (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize())),
                                                   gameObjects[i][j]->GetPosition().second);
                }
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                                   gameObjects[i][j]->GetPosition().second - gameObjects[0][0]->GetPosition().second);
                }
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize() < SCREEN_HEIGHT) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                if (gameObjects[i][j]->GetResizable()) {
                    gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                                   gameObjects[i][j]->GetPosition().second + (SCREEN_HEIGHT - (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize())));
                }
            }
        }
    }
}

void HandleEvents(Input* playerInput) {
    SDL_Event e;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    while( SDL_PollEvent( &e ) != 0 )
    {
        switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEWHEEL:
                if (e.wheel.preciseY > 0 || e.wheel.preciseY < 0) {
                    if (selectedObject == nullptr) {
                        zoom(e, playerInput);
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                playerInput->MouseButtonDown(e.button);
                if (playerInput->GetMouseButtonDown("Left")) {
                    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                    selectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                    if (selectedObject == nullptr) {
                        selectedObject = selectItem(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                        if (selectedObject == nullptr) {
                            selectedObject = selectPiece(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                            if (selectedObject != nullptr && selectedObject->type == GameObject::PIECE) {
                                Piece* piece = dynamic_cast<Piece *>(selectedObject);
                                int centerX = piece->GetCenter().first;
                                int centerY = piece->GetCenter().second;
                                piece->SetScale(piece->GetScale()*2);
                                piece->SetCenter(centerX, centerY - (piece->GetDimensions().second/2) * piece->GetSize());
                                piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetCenter().second);
                                if (piece->GetCurrentAnimation() != NULL) piece->GetCurrentAnimation()->Pause();
                            } else {
                                //selectedObject = selectObject(gameObjects, playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                            }
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (selectedObject != nullptr) {
                    if (selectedObject->GetName() == "reset button") {
                        ResetMap();
                    } else if ((selectedObject->GetName() == "dieOne" ||
                                selectedObject->GetName() == "dieTwo") && movesLeft < 1) {
                        currentRoll = Roll();
                        movesLeft = currentRoll;
                    } else if (selectedObject->GetName() == "finish turn button") {
                        FinishTurn();

                    } else if (selectedObject->GetName() == "claim peak button") {
                        ClaimPeak(dynamic_cast<UIElement *>(selectedObject));
                        RefreshClaimNotifs();

                    }else if (selectedObject->type == GameObject::PIECE) {
                        Piece* piece = dynamic_cast<Piece *>(selectedObject);
                        int centerX = piece->GetCenter().first;
                        int centerY = piece->GetCenter().second;


                        Terrain* startingTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second + (piece->GetDimensions().second/2) * piece->GetSize() * piece->GetScale());
                        Terrain* targetTerrain = selectTerrain(piece->GetCenter().first, centerY + (piece->GetDimensions().second/2) * piece->GetSize() * piece->GetScale());
                        piece->SetScale(piece->GetScale()*0.5f);
                        piece->SetCenter(centerX, centerY + (piece->GetDimensions().second/2) * piece->GetSize());
                        if (piece->GetCurrentAnimation() != NULL) piece->GetCurrentAnimation()->Unpause();
                        Move(piece, startingTerrain, targetTerrain, movesLeft);
                        hoveringTerrain = nullptr;
                        if (targetTerrain != NULL) std::cout <<targetTerrain->GetBiome() << std::endl;

                    } else if (selectedObject->type == GameObject::ITEM) {
                        std::cout << "item selected: " << selectedObject->GetName() << std::endl;
                    }
                }
                playerInput->MouseButtonUp(e.button);
                selectedObject = nullptr;
                break;
        }
    }
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    if (playerInput->GetMouseButtonDown("Middle")) {
        scroll(playerInput);
    }
    if (playerInput->GetMouseButtonDown("Left")) {
        if (selectedObject != nullptr) {
            moveSelectedObject(selectedObject, playerInput);
        }
    }
    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}

//-------------------//

void GeneratePeak() {
    int x = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
    int y = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
    int shape = rand() % (int)(shapeCount);
    int height = (rand() % rules->GetMaxHeight()) + 1;
    if (rules->GetRemainingPoints() <= rules->GetMaxHeight()) height = rules->GetRemainingPoints();
    rules->SetRemainingPoints(rules->GetRemainingPoints() - height);
    string name = to_string(height) + ", " + to_string(peaks.size() + 1);
    Peak* peak = new Peak(name, textures[to_string(shape)][height], surfaces[to_string(shape)], false, true, height, renderer);
    double rotation = rand() % 360;
    peak->SetRotation(rotation);
    peak->SetScale(0.1);
    peak->SetCenter(x, y);
    peaks.push_back(peak);
    peak->SetPeakID(peaks.size());
    terrain[height].push_back(peak);
    gameObjects[height].push_back(peak);

    string outlineName = "outline " + to_string(shape);
    GameObject* outline = new GameObject(outlineName, textures[outlineName][height], surfaces[outlineName], false, true);
    outline->SetScale(0.1);
    outline->SetCenter(x,y);
    terrainOutlines[height].push_back(outline);
    gameObjects[height].push_back(outline);
    peak->SetOutline(outline);

    int itemChance = (rand() % 100) + 1;
    float odds = (float)height / (float)rules->GetMaxHeight() * 100;
    if (odds > itemChance) {
        if (rules->GetRemainingItems() > 0) {
            rules->SetRemainingItems(rules->GetRemainingItems() - 1);
            int itemID = rand() % 3;
            string itemName;
            if (itemID == 0) itemName = "playerOne zipline";
            else if (itemID == 1) itemName = "parachute";
            else if (itemID == 2) itemName = "buddy rope";
            Item* item = new Item(itemName, textures[itemName][0], surfaces[itemName], true);
            pieces.push_back(item);
            item->SetScale(0.05);
            item->SetCenter(peak->GetCenter().first, peak->GetCenter().second);
            gameObjects[height].push_back(item);
            peak->SetItem(item);
        }
    }

    while (MergeTerrain(peak).size() != 0) {
        int a = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
        int b = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
        peak->SetCenter(a, b);
        outline->SetCenter(a, b);
        if (peak->GetItem() != nullptr) {
            peak->GetItem()->SetCenter(a, b);
        }
    }
    SDL_Color color;
    if (height == 1) {
        SDL_SetTextureColorMod(peak->GetTexture(), 246, 215, 176);
        color = {246, 215, 176};
    } else {
        SDL_SetTextureColorMod(peak->GetTexture(), 200/rules->GetMaxHeight() * peak->GetLayer(), std::min(200/rules->GetMaxHeight() * peak->GetLayer() * 2, 255), 200/rules->GetMaxHeight() * peak->GetLayer());
        color = {static_cast<Uint8>(200/rules->GetMaxHeight() * peak->GetLayer()), static_cast<Uint8>(std::min(200/rules->GetMaxHeight() * peak->GetLayer() * 2, 255)), static_cast<Uint8>(200/rules->GetMaxHeight() * peak->GetLayer())};
    }
    peak->SetColor(color);
    GenerateTerrain(peak, shape, height);
}

void GenerateTerrain(Peak* peak, int shape, int height) {
    for (int i = height - 1; i > 0; i--) {
        string name = to_string(i) + ", " + to_string(peak->GetPeakID());
        //shape = rand() % (int)(15);


        Terrain* layer = new Terrain(name, textures[to_string(shape)][i], surfaces[to_string(shape)], false, true, i, renderer);
        layer->SetScale(peak->GetScale() + 0.1 * (height - i));
        layer->SetRotation(peak->GetRotation());
        layer->SetPeak(peak);


        Terrain* above;
        if (peak->childTerrain.size() == 0) {
            above = peak;
        } else {
            above = peak->childTerrain[peak->childTerrain.size() - 1];
        }
        int offsetX = rand() % 20;
        int offsetY = rand() % 20;
        int negativeX = rand() % 2;
        int negativeY = rand() % 2;
        if (negativeX == 1) offsetX *= -1;
        if (negativeY == 1) offsetY *= -1;
        layer->SetCenter(above->GetCenter().first + offsetX, above->GetCenter().second + offsetY);
        while (!TerrainIsSurrounded(above, layer)) {
            offsetX = rand() % 20;
            offsetY = rand() % 20;
            negativeX = rand() % 2;
            negativeY = rand() % 2;
            if (negativeX == 1) offsetX *= -1;
            if (negativeY == 1) offsetY *= -1;
            layer->SetCenter(above->GetCenter().first + offsetX, above->GetCenter().second + offsetY);
        }
        layer->SetOffsetX(offsetX);
        layer->SetOffsetY(offsetY);
        SDL_Color color;
        if (i == 1) {
            SDL_SetTextureColorMod(layer->GetTexture(), 246, 215, 176);
            color = {246, 215, 176};

        } else {
            SDL_SetTextureColorMod(layer->GetTexture(), 200/rules->GetMaxHeight() * layer->GetLayer(), std::min(200/rules->GetMaxHeight() * layer->GetLayer() * 2, 255), 200/rules->GetMaxHeight() * layer->GetLayer());
            color = {static_cast<Uint8>(200/rules->GetMaxHeight() * layer->GetLayer()), static_cast<Uint8>(std::min(200/rules->GetMaxHeight() * layer->GetLayer() * 2, 255)), static_cast<Uint8>(200/rules->GetMaxHeight() * layer->GetLayer())};
        }
        layer->SetColor(color);
        gameObjects[i].push_back(layer);
        terrain[i].push_back(layer);
        peak->childTerrain.push_back(layer);

        string outlineName = "outline " + to_string(shape);
        GameObject* outline = new GameObject(outlineName, textures[outlineName][i], surfaces[outlineName], false, true);
        outline->SetScale(peak->GetScale() + 0.1 * (height - i));
        outline->SetCenter(peak->GetCenter().first,peak->GetCenter().second);
        terrainOutlines[i].push_back(outline);
        gameObjects[i].push_back(outline);
        layer->SetOutline(outline);

    }
}

std::vector<Terrain*> MergeTerrain(Terrain* peak) {
    std::vector<Terrain*> connectedTerrain;
    for (int k = 0; k < terrain[peak->GetLayer()].size(); k++) {
        Terrain* other = terrain[peak->GetLayer()][k];
        if (peak == other) continue;
        bool intersecting = false;
        bool topLeftOne = false;
        bool topRightOne = false;
        bool bottomLeftOne = false;
        bool bottomRightOne = false;
        bool topLeftTwo = false;
        bool topRightTwo = false;
        bool bottomLeftTwo = false;
        bool bottomRightTwo = false;
        pair<int, int> offsetOne;
        pair<int, int> offsetTwo;
        int length = 0;
        int height = 0;
        SDL_Color pixelOne;
        SDL_Color pixelTwo;

        //top left
        if (peak->GetPosition().first > other->GetPosition().first &&
             peak->GetPosition().first < other->GetBottomRight().first &&
             peak->GetPosition().second > other->GetPosition().second &&
             peak->GetPosition().second < other->GetBottomRight().second) {

            topLeftOne = true;


        } if (peak->GetBottomRight().first > other->GetPosition().first &&
             peak->GetBottomRight().first < other->GetBottomRight().first &&
             peak->GetPosition().second > other->GetPosition().second &&
             peak->GetPosition().second < other->GetBottomRight().second) {

            topRightOne = true;


        }if (peak->GetPosition().first > other->GetPosition().first &&
             peak->GetPosition().first < other->GetBottomRight().first &&
             peak->GetBottomRight().second > other->GetPosition().second &&
             peak->GetBottomRight().second < other->GetBottomRight().second) {

            bottomLeftOne = true;

        } if (peak->GetBottomRight().first > other->GetPosition().first &&
             peak->GetBottomRight().first < other->GetBottomRight().first &&
             peak->GetBottomRight().second > other->GetPosition().second &&
             peak->GetBottomRight().second < other->GetBottomRight().second) {

            bottomRightOne = true;
        }


        if (other->GetPosition().first > peak->GetPosition().first &&
            other->GetPosition().first < peak->GetBottomRight().first &&
            other->GetPosition().second > peak->GetPosition().second &&
            other->GetPosition().second < peak->GetBottomRight().second) {

            topLeftTwo = true;


        } if (other->GetBottomRight().first > peak->GetPosition().first &&
              other->GetBottomRight().first < peak->GetBottomRight().first &&
              other->GetPosition().second > peak->GetPosition().second &&
              other->GetPosition().second < peak->GetBottomRight().second) {

            topRightTwo = true;

        }if (other->GetPosition().first > peak->GetPosition().first &&
             other->GetPosition().first < peak->GetBottomRight().first &&
             other->GetBottomRight().second > peak->GetPosition().second &&
             other->GetBottomRight().second < peak->GetBottomRight().second) {

            bottomLeftTwo = true;

        } if (other->GetBottomRight().first > peak->GetPosition().first &&
              other->GetBottomRight().first < peak->GetBottomRight().first &&
              other->GetBottomRight().second > peak->GetPosition().second &&
              other->GetBottomRight().second < peak->GetBottomRight().second) {

            bottomRightTwo = true;
        }

        if (!topLeftOne && !topRightOne && !bottomLeftOne && !bottomRightOne && !topLeftTwo && !topRightTwo && !bottomLeftTwo && !bottomRightTwo) {
            continue;
        } else if (topLeftOne && !topRightOne && bottomLeftOne && !bottomRightOne) {
            //std::cout << "One" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftOne && !topRightOne && bottomLeftOne && bottomRightOne) {
            //std::cout << "Two" << std::endl;
            length = peak->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = 0;

        } else if (!topLeftOne && topRightOne && !bottomLeftOne && bottomRightOne) {
            //std::cout << "Three" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = peak->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = 0;
            offsetTwo.first = 0;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (topLeftOne && topRightOne && !bottomLeftOne && !bottomRightOne) {
            //std::cout << "Four" << std::endl;
            length = peak->GetBottomRight().first - peak->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (topLeftOne && !topRightOne && !bottomLeftOne && !bottomRightOne) {
            //std::cout << "Five" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftOne && topRightOne && !bottomLeftOne && !bottomRightOne) {
            //std::cout << "Six" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = 0;
            offsetTwo.first = 0;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftOne && !topRightOne && bottomLeftOne && !bottomRightOne) {
            //std::cout << "Seven" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = 0;

        } else if (!topLeftOne && !topRightOne && !bottomLeftOne && bottomRightOne) {
            //std::cout << "Eight" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;


        } else if (topLeftOne && topRightOne && bottomLeftOne && bottomRightOne) {
            //std::cout << "Nine" << std::endl;
            length = peak->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;


        } else if (topLeftTwo && topRightTwo && bottomLeftTwo && bottomRightTwo) {
            //std::cout << "Ten" << std::endl;
            length = other->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;

        } else if (topLeftTwo && !topRightTwo && bottomLeftTwo && !bottomRightTwo) {
            //std::cout << "Eleven" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;

        } else if (!topLeftTwo && !topRightTwo && bottomLeftTwo && bottomRightTwo) {
            //std::cout << "Twelve" << std::endl;
            length = other->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = 0;
            offsetTwo.first = 0;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftTwo && topRightTwo && !bottomLeftTwo && bottomRightTwo) {
            //std::cout << "Thirteen" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = other->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = 0;

        } else if (topLeftTwo && topRightTwo && !bottomLeftTwo && !bottomRightTwo) {
            //std::cout << "Fourteen" << std::endl;
            length = other->GetBottomRight().first - other->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;
        }

        for (int i = 0; i < length; i++) {
            for (int j = 0; j < height; j++) {
                pixelOne = GetPixelColor(peak->GetSurface(),  (offsetOne.first + i) / (peak->GetScale() * peak->GetSize()) , (offsetOne.second + j) / (peak->GetScale() * peak->GetSize()));
                pixelTwo = GetPixelColor(other->GetSurface(), (offsetTwo.first + i) / (other->GetScale() * other->GetSize()), (offsetTwo.second + j) / (other->GetScale() * other->GetSize()));



                if ((pixelOne.r == 255 && pixelOne.g == 255 && pixelOne.b == 255) && (pixelTwo.r == 255 && pixelTwo.g == 255 && pixelTwo.b == 255)) {
                    intersecting = true;
                    break;
                }
            }
            if (intersecting) break;
        }

        if (intersecting) {
            connectedTerrain.push_back(other);
        }

    }

    return connectedTerrain;
}

bool TerrainIsSurrounded(Terrain* peak, Terrain* other) {
    bool surrounded = true;
    bool topLeftOne = false;
    bool topRightOne = false;
    bool bottomLeftOne = false;
    bool bottomRightOne = false;
    bool topLeftTwo = false;
    bool topRightTwo = false;
    bool bottomLeftTwo = false;
    bool bottomRightTwo = false;
    pair<int, int> offsetOne;
    pair<int, int> offsetTwo;
    int length = 0;
    int height = 0;
    SDL_Color pixelOne;
    SDL_Color pixelTwo;

    //top left
    if (peak->GetPosition().first > other->GetPosition().first &&
        peak->GetPosition().first < other->GetBottomRight().first &&
        peak->GetPosition().second > other->GetPosition().second &&
        peak->GetPosition().second < other->GetBottomRight().second) {

        topLeftOne = true;


    } if (peak->GetBottomRight().first > other->GetPosition().first &&
          peak->GetBottomRight().first < other->GetBottomRight().first &&
          peak->GetPosition().second > other->GetPosition().second &&
          peak->GetPosition().second < other->GetBottomRight().second) {

        topRightOne = true;


    }if (peak->GetPosition().first > other->GetPosition().first &&
         peak->GetPosition().first < other->GetBottomRight().first &&
         peak->GetBottomRight().second > other->GetPosition().second &&
         peak->GetBottomRight().second < other->GetBottomRight().second) {

        bottomLeftOne = true;

    } if (peak->GetBottomRight().first > other->GetPosition().first &&
          peak->GetBottomRight().first < other->GetBottomRight().first &&
          peak->GetBottomRight().second > other->GetPosition().second &&
          peak->GetBottomRight().second < other->GetBottomRight().second) {

        bottomRightOne = true;
    }

    if (topLeftOne && topRightOne && bottomLeftOne && bottomRightOne) {
        length = peak->GetBottomRight().first - peak->GetPosition().first;
        height = peak->GetBottomRight().second - peak->GetPosition().second;
        offsetOne.first = 0;
        offsetOne.second = 0;
        offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
        offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;
    } else return false;

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < height; j++) {
            pixelOne = GetPixelColor(peak->GetSurface(),  (offsetOne.first + i) / (peak->GetScale() * peak->GetSize()) , (offsetOne.second + j) / (peak->GetScale() * peak->GetSize()));
            pixelTwo = GetPixelColor(other->GetSurface(), (offsetTwo.first + i) / (other->GetScale() * other->GetSize()), (offsetTwo.second + j) / (other->GetScale() * other->GetSize()));



            if ((pixelOne.r == 255 && pixelOne.g == 255 && pixelOne.b == 255) && (pixelTwo.r != 255 || pixelTwo.g != 255 || pixelTwo.b != 255)) {
                surrounded = false;
                break;
            }
        }
        if (!surrounded) break;
    }

    return surrounded;
}


void GroomTerrain() {
    int moveCount = 1;
    while (moveCount > 0) {
        moveCount = 0;
        for (int i = 0; i < peaks.size(); i++) {
            while (MergeTerrain(peaks[i]).size() != 0) {
                int a = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
                int b = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
                peaks[i]->SetCenter(a, b);
                if (peaks[i]->GetItem() != nullptr) {
                    peaks[i]->GetItem()->SetCenter(a, b);
                }
                moveCount++;
            }
            Terrain* above = nullptr;
            Terrain* current = nullptr;
            for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
                current = peaks[i]->childTerrain[j];
                if (j == 0) {
                    above = peaks[i];
                } else {
                    above = peaks[i]->childTerrain[j - 1];
                }
                current->SetCenter(above->GetCenter().first + current->GetOffsetX(),above->GetCenter().second + current->GetOffsetY());
            }
        }
    }
    ConnectTerrain();
    NeighborTerrain();
    SetTerrainBiome();

}

void ConnectTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->connectedTerrain = MergeTerrain(terrain[i][j]);
        }
    }
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            std::vector<Terrain*> neighboringTerrain;
            ConnectConnectedTerrain(neighboringTerrain, terrain[i][j]);
            terrain[i][j]->connectedTerrain = neighboringTerrain;
        }
    }
}

void ConnectConnectedTerrain(std::vector<Terrain*>& connectedTerrain, Terrain* currentTerrain) {
    for (int i = 0; i < currentTerrain->connectedTerrain.size(); i++) {

        bool alreadyCovered = false;

        for (int j = 0; j < connectedTerrain.size(); j++) {
            if (connectedTerrain[j] == currentTerrain->connectedTerrain[i]) {
                alreadyCovered = true;
            }
        }
        if (alreadyCovered) continue;

        connectedTerrain.push_back(currentTerrain->connectedTerrain[i]);
        ConnectConnectedTerrain(connectedTerrain, currentTerrain->connectedTerrain[i]);
    }
}

void NeighborTerrain() {
    for (int i = 0; i < peaks.size(); i++) {
        for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
            if (j == 0) {
                peaks[i]->childTerrain[j]->SetUpperTerrain(peaks[i]);
                peaks[i]->SetLowerTerrain(peaks[i]->childTerrain[j]);
            } else {
                peaks[i]->childTerrain[j]->SetUpperTerrain(peaks[i]->childTerrain[j - 1]);
            }
            if (j == peaks[i]->childTerrain.size() - 1) {
                peaks[i]->childTerrain[j]->SetLowerTerrain(nullptr);
            } else {
                peaks[i]->childTerrain[j]->SetLowerTerrain(peaks[i]->childTerrain[j + 1]);
            }

        }
    }
}

void SetTerrainBiome() {
    for (int i = 0; i < peaks.size(); i++) {
        if (peaks[i]->GetBiome() != "") continue;
        int biomeID = rand() % 3;
        if (peaks[i]->GetLayer() == 1 && peaks[i]->connectedTerrain.size() == 0) biomeID = 0;
        string biome;
        switch (biomeID) {
            case 0:
                biome = "plains";
                break;
            case 1:
                biome = "stone";
                break;
            case 2:
                biome = "glacier";
                break;
            default:
                biome = "plains";
                break;
        }
        peaks[i]->SetBiome(biome);
        for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
            peaks[i]->childTerrain[j]->SetBiome(biome);
            for (int k = 0; k < peaks[i]->childTerrain[j]->connectedTerrain.size(); k++) {
                Terrain* tempTerrain = peaks[i]->childTerrain[j]->connectedTerrain[k];
                while (tempTerrain != nullptr) {
                    tempTerrain->SetBiome(biome);
                    tempTerrain = tempTerrain->GetUpperTerrain();
                }
            }
        }
    }
}

//-------------------//

void Move(Piece* piece, Terrain* startingPoint, Terrain* targetTerrain, int& movesLeft) {

    int moveCount = 0;
    int heightDifference;
    std::vector<Terrain*> currentPath;

    bool moveSuccess = MovementAttempt(heightDifference, moveCount, startingPoint, targetTerrain, currentPath, false);

    if (!moveSuccess) {
        piece->SetCenter(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second + piece->GetDimensions().second/2 * piece->GetSize());
    } else {
        movesLeft = movesLeft - moveCount;
        piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetCenter().second);
        piece->SetOccupyingTerrain(targetTerrain);
        if (startingPoint != NULL) {
            for (int i = 0; i < startingPoint->occupants.size(); i++) {
                if (startingPoint->occupants[i] == piece) {
                    startingPoint->occupants.erase(startingPoint->occupants.begin() + i);
                }
            }
        }
        if (targetTerrain != NULL) targetTerrain->occupants.push_back(piece);
        if (targetTerrain != NULL && targetTerrain->type == GameObject::PEAK) {
            RefreshClaimNotifs(dynamic_cast<Peak *>(targetTerrain));
        } else {
            RefreshClaimNotifs();
        }
        if (targetTerrain == NULL)  {
            piece->SetCurrentAnimation(piece->animations["floatIdle"]);
            piece->SetSurface(surfaces[currentTurn + " piece float"]);
        }
        else {
            piece->SetCurrentAnimation(piece->animations["saluteIdle"]);
            piece->SetSurface(surfaces[currentTurn + " piece salute"]);
        }
    }




    UpdateMovesLeft();

}

bool MovementAttempt(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain, std::vector<Terrain*>& currentPath, bool fromAdjacent) {

    int targetTerrainLayer;
    int currentTerrainLayer;
    if (targetTerrain == NULL) targetTerrainLayer = 0;
    else targetTerrainLayer = targetTerrain->GetLayer();
    if (currentTerrain == NULL) currentTerrainLayer = 0;
    else currentTerrainLayer =  currentTerrain->GetLayer();

    heightDifference = targetTerrainLayer - currentTerrainLayer;

    if (heightDifference == 0) {
        if (currentTerrain == targetTerrain && attemptedMoves <= movesLeft) return true;
    }

    if (currentTerrainLayer == 0) {
        currentPath.push_back(GetTargetTerrainBase(targetTerrain));
        return MovementAttempt(heightDifference, ++attemptedMoves, GetTargetTerrainBase(targetTerrain), targetTerrain, currentPath, false);
    }

    if (heightDifference > 0) {
        if (DirectMovementUp(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;
    }

    if (heightDifference < 0) {
        if (DirectMovementDown(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;
    }
    if (!fromAdjacent) {
        if (AdjacentMovement(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;
    }

    if (currentTerrain->GetLowerTerrain() != NULL && !fromAdjacent) {

        currentPath.push_back(currentTerrain->GetLowerTerrain());

        attemptedMoves++;
        if (MovementAttempt(heightDifference, attemptedMoves, currentTerrain->GetLowerTerrain(), targetTerrain, currentPath, false) && attemptedMoves <= movesLeft) return true;


    } else if (!fromAdjacent) {

        attemptedMoves = attemptedMoves + 2;
        if (attemptedMoves > movesLeft) return false;
        currentPath.push_back(nullptr);
        currentPath.push_back(GetTargetTerrainBase(targetTerrain));
        Terrain* terrainBase = GetTargetTerrainBase(targetTerrain);
        if (MovementAttempt(heightDifference, attemptedMoves, terrainBase, targetTerrain, currentPath, false) && attemptedMoves <= movesLeft) return true;
    }

    return false;

}

bool DirectMovementUp(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath) {


    int i;
    for (i = 0; i < heightDifference; i++) {
        if (currentTerrain->GetUpperTerrain() == NULL) break;
        currentTerrain = currentTerrain->GetUpperTerrain();
        currentPath.push_back(currentTerrain);
        attemptedMoves++;
        if (currentTerrain == targetTerrain) break;
    }


    if (currentTerrain == targetTerrain) return true;
    else {
        for (int j = 0; j < i; j++) {
            currentPath.pop_back();
            attemptedMoves--;

        }
    }
    return false;
}

bool DirectMovementDown(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath) {
    int i;
    for (i = 0; i > heightDifference; i--) {
        currentTerrain = currentTerrain->GetLowerTerrain();
        currentPath.push_back(currentTerrain);
        attemptedMoves++;
    }
    if (currentTerrain == targetTerrain) return true;
    else {
        for (int j = 0; j > i; j--) {
            currentPath.pop_back();
            attemptedMoves--;

        }
    }
    return false;
}

Terrain* GetTargetTerrainBase(Terrain* targetTerrain) {
    if (targetTerrain == NULL) return targetTerrain;
    Terrain* targetTerrainBase = targetTerrain;
    while (targetTerrainBase->GetLowerTerrain() != nullptr) {
        targetTerrainBase = targetTerrainBase->GetLowerTerrain();
    }
    return targetTerrainBase;
}

bool AdjacentMovement(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath) {
    for (int i = 0; i < currentTerrain->connectedTerrain.size(); i++) {

        bool alreadyCovered = false;

        for (int j = 0; j < currentPath.size(); j++) {
            if (currentPath[j] == currentTerrain->connectedTerrain[i]) {
                alreadyCovered = true;
            }
        }
        if (alreadyCovered) continue;

        currentPath.push_back(currentTerrain->connectedTerrain[i]);

        if (MovementAttempt(heightDifference, attemptedMoves, currentTerrain->connectedTerrain[i], targetTerrain, currentPath, true) && attemptedMoves <= movesLeft) return true;
        else currentPath.pop_back();
    }

    return false;
}

//-------------------//

int Roll(){
    int rollOne = (rand() % 6) + 1;
    string rollOneStr = "die " + to_string(rollOne);
    int rollTwo = (rand() % 6) + 1;
    string rollTwoStr = "die " + to_string(rollTwo);
    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetName() == "dieOne") {
            uiElements[i]->SetTexture(textures[rollOneStr][0]);
        } else if (uiElements[i]->GetName() == "dieTwo") {
            uiElements[i]->SetTexture(textures[rollTwoStr][0]);
        }
    }
    movesLeft = rollOne + rollTwo;

    UpdateMovesLeft();

    return movesLeft;
}

void RotateTurn() {

    if (currentTurn == "playerOne") {
        playerOneText->SetRendered(false);
        currentTurn = "playerTwo";
        playerTwoText->SetRendered(true);
        currentInventory = playerTwoInventory;
    }
    else if (currentTurn == "playerTwo") {
        playerTwoText->SetRendered(false);
        currentTurn = "playerThree";
        playerThreeText->SetRendered(true);
        currentInventory = playerThreeInventory;
    }
    else if (currentTurn == "playerThree") {

        playerThreeText->SetRendered(false);
        currentTurn = "playerFour";
        playerFourText->SetRendered(true);
        currentInventory = playerFourInventory;
    }
    else if (currentTurn == "playerFour") {
        playerFourText->SetRendered(false);
        currentTurn = "playerOne";
        playerOneText->SetRendered(true);
        currentInventory = playerOneInventory;
    }

    for (int i = 0; i < pieces.size(); i++) {
        if (currentTurn == pieces[i]->GetName()) {
            pieces[i]->SetSelectable(true);
        } else pieces[i]->SetSelectable(false);
    }

}

void UpdateMovesLeft() {
    string movesLeftStr = "moves left " + to_string(movesLeft);

    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetName() == "movesLeftCount") {
            uiElements[i]->SetTexture(textures[movesLeftStr][0]);
        }
    }
}

void RefreshClaimNotifs(Peak* individualPeak) {
    if (individualPeak != NULL) {
        for (int i = 0; i < individualPeak->occupants.size(); i++) {
            if (individualPeak->occupants[i]->GetName() == currentTurn && individualPeak->GetClaimedBy() != currentTurn) {
                individualPeak->GetClaimNotif()->SetRendered(true);
                break;
            }
            else {
                individualPeak->GetClaimNotif()->SetRendered(false);
            }
        }
    } else {
        for (int i = 0; i < peaks.size(); i++) {
            for (int j = 0; j < peaks[i]->occupants.size(); j++) {
                if (peaks[i]->occupants[j]->GetName() == currentTurn && peaks[i]->GetClaimedBy() != currentTurn) {
                    peaks[i]->GetClaimNotif()->SetRendered(true);
                    break;
                } else {
                    peaks[i]->GetClaimNotif()->SetRendered(false);
                }
            }
            if (peaks[i]->occupants.size() == 0) peaks[i]->GetClaimNotif()->SetRendered(false);
        }
    }
}

void FinishTurn() {
    RotateTurn();
    movesLeft = 0;
    UpdateMovesLeft();
    RefreshClaimNotifs();
}

void ClaimPeak(UIElement* peakNotif) {
    Peak* peak = peakNotif->GetAssociatedPeak();

    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetName() != currentTurn) {
            PeakBattle(peak, currentTurn, peak->occupants[i]->GetName());
        }
    }
    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetName() == currentTurn) {
            peak->Claim(currentTurn);
            if (peak->GetItem() != nullptr) {
                peak->GetItem()->SetOwner(currentTurn);
                peak->GetItem()->SetResizable(false);
                peak->GetItem()->SetSelectable(true);
                currentInventory.push_back(peak->GetItem());
                if (currentTurn == "playerOne") playerOneInventory.push_back(peak->GetItem());
                else if (currentTurn == "playerTwo") playerTwoInventory.push_back(peak->GetItem());
                else if (currentTurn == "playerThree") playerThreeInventory.push_back(peak->GetItem());
                else if (currentTurn == "playerFour") playerFourInventory.push_back(peak->GetItem());
                peak->SetItem(nullptr);
                for (int j = 0; j < currentInventory.size(); j++) {
                    currentInventory[j]->SetScale(0.1);
                    currentInventory[j]->SetPosition(100 + 70*j, 10);
                }
            }
            for (int j = 0; j < peak->flags.size(); j++) {
                if (peak->flags[j]->GetName() == currentTurn) {
                    peak->flags[j]->SetRendered(true);
                } else {
                    peak->flags[j]->SetRendered(false);
                }
            }
            break;
        }
    }

    UpdateScore();

}

void PeakBattle(Peak* peak, string attackingTeam, string defendingTeam) {
    int attackers = 0;
    int attackPower = 0;
    int defenders = 0;
    int defensePower = 0;
    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetName() == attackingTeam) {
            attackers++;
        } else if (peak->occupants[i]->GetName() == defendingTeam) {
            defenders++;
        }
    }
    for (int i = 0; i < attackers; i++) {
        attackPower += rand() % 6 + 1;
    }
    for (int i = 0; i < defenders; i++) {
        defensePower += rand() % 6 + 1;
    }
    std::cout << "Attack power: " << attackPower << std::endl;
    std::cout << "Defense power: " << defensePower << std::endl;
    if (attackPower >= defensePower) {
        Retreat(peak, defendingTeam);
    } else {
        Retreat(peak, attackingTeam);
    }
}

void Retreat(Peak* peak, string retreatingTeam) {
    for (int i = 0; i < peak->occupants.size(); i++) {
        if (peak->occupants[i]->GetName() == retreatingTeam) {

            int x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
            int y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
            while (selectTerrain(x, y) != NULL) {
                x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
                y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
            }
            peak->occupants[i]->SetCenter(x, y);
            peak->occupants[i]->SetCurrentAnimation(peak->occupants[i]->animations["floatIdle"]);
            peak->occupants.erase(peak->occupants.begin() + i);
        }
    }
}

void UpdateScore() {
    playerOneScore = 0;
    playerTwoScore = 0;
    playerThreeScore = 0;
    playerFourScore = 0;
    int peaksLeft = peaks.size();
    for (int i = 0; i < peaks.size(); i++) {
        if (peaks[i]->GetClaimedBy() == "playerOne") {
            peaksLeft--;
            playerOneScore += peaks[i]->GetLayer();
        } else if (peaks[i]->GetClaimedBy() == "playerTwo") {
            peaksLeft--;
            playerTwoScore += peaks[i]->GetLayer();
        } else if (peaks[i]->GetClaimedBy() == "playerThree") {
            peaksLeft--;
            playerThreeScore += peaks[i]->GetLayer();
        } else if (peaks[i]->GetClaimedBy() == "playerFour") {
            peaksLeft--;
            playerFourScore += peaks[i]->GetLayer();
        }
    }
    if (peaksLeft == 0) {
        if (playerOneScore > playerTwoScore && playerOneScore > playerThreeScore && playerOneScore > playerFourScore) {
            GameFinished("playerOne");
        } else if (playerTwoScore > playerOneScore && playerTwoScore > playerThreeScore && playerTwoScore > playerFourScore) {
            GameFinished("playerTwo");
        } else if (playerThreeScore > playerOneScore && playerThreeScore > playerTwoScore && playerThreeScore > playerFourScore) {
            GameFinished("playerThree");
        } else if (playerFourScore > playerOneScore && playerFourScore > playerTwoScore && playerFourScore > playerThreeScore) {
            GameFinished("playerFour");
        } else {
            Tiebreaker();
        }

    }
    std::cout << "Player One: " << playerOneScore << endl;
    std::cout << "Player Two: " << playerTwoScore << endl;
    std::cout << "Player Three: " << playerThreeScore << endl;
    std::cout << "Player Four: " << playerFourScore << endl;

}

void GameFinished(string winner) {
    string winnerColor;
    if (winner == "playerOne") {
        winnerColor = "winnerRed";
    } else if (winner == "playerTwo") {
        winnerColor = "winnerGreen";
    } else if (winner == "playerThree") {
        winnerColor = "winnerBlue";
    } else if (winner == "playerFour") {
        winnerColor = "winnerYellow";
    }

    UIElement* winnerMessage = new UIElement("winner message", textures[winnerColor][0], surfaces[winnerColor], true);
    winnerMessage->SetCenter(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    uiElements.push_back(winnerMessage);
    gameObjects[gameObjects.size()-1].push_back(winnerMessage);
}

void Tiebreaker() {

}

void GeneratePixels() {


    for (int i = 0; i < SCREEN_WIDTH/2; i++) {
        for (int j = 0; j < SCREEN_HEIGHT/2; j++) {
            int x = i*2 + 1;
            int y = j*2 + 1;
            Terrain* currentTerrain = selectTerrain(x, y);
            if (currentTerrain == nullptr) continue;

            Pixel* pixel = new Pixel("pixel", textures["pixel"][0], surfaces["pixel"], false, true);
            pixel->SetHiddenTerrain(selectTerrain(x, y));

            y += 2;
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

            pixel->SetWidth(width + 1);
            pixel->SetHeight(height + 1);

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

//-------------------//

void AnimationHandler(float fps, Uint64& lastFrame, Uint64& lastUpdate) {
    Uint64 current = SDL_GetTicks();
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetCurrentAnimation() != nullptr) {
                gameObjects[i][j]->GetCurrentAnimation()->CycleFrame(current);
            }

        }
    }




}