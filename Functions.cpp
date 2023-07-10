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

std::vector<std::vector<GameObject*>> terrainOutlines;

std::vector<UIElement*> uiElements;

std::vector<Piece*> pieces;

GameObject* selectedObject = nullptr;

Item* selectedItem = nullptr;

GameRules* rules = new GameRules();

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
    GameObject* map = new GameObject("map", textures["map"][0], surfaces["map"], false, true);


    SDL_SetTextureColorMod(map->GetTexture(), 0, 0, 0);
    map->SetPosition(0, 0);


    for (int i = 0; i < rules->GetMaxHeight() + 1; i++) {
        gameObjects.push_back(vector<GameObject *>());
        terrain.push_back(vector<Terrain*>());
        terrainOutlines.push_back(vector<GameObject*>());
        //generate map layers
    }

    gameObjects[0].push_back(map);
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
            flag->SetScale(0.1);
            flag->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second - 20);
            pieces.push_back(flag);
            gameObjects[gameObjects.size() - 2].push_back(flag);
            peaks[i]->flags.push_back(flag);

        }
        string pieceName = playerNumber + " piece";
        int x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
        int y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
        while (selectTerrain(x, y) != NULL || selectUI(x, y) != NULL) {
            x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
            y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
        }
        for (int i = 0; i < rules->GetPieces(); i++) {
            Piece* piece = new Piece(playerNumber, textures[pieceName][0], surfaces[pieceName], true);
            gameObjects[rules->GetMaxHeight() + 1].push_back(piece);
            pieces.push_back(piece);
            piece->SetScale((float) 1/22);
            piece->SetCenter(x, y);
            piece->SetDesignatedLocation(x, y);
            piece->SetSelectable(selectable);
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

    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            delete gameObjects[i][j];
        }
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
    loadGamePieces();
    loadUI();
    currentTurn = "playerOne";


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
    DIR* dp;
    int i = 0;
    struct dirent* ep;
    dp = opendir("Images");
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (ep->d_namlen == 1 || ep->d_namlen == 2) continue;
            else {
                std::string fileName = ep->d_name;
                std::size_t pos = fileName.find(".");
                std::string shorthand = fileName.substr(0,pos);
                std::string texturePath = "Images/" + fileName;

                for (int j = 0; j < rules->GetMaxHeight()+1; j++) {
                    textures[shorthand][j] = loadTexture(texturePath);
                }

                surfaces[shorthand] = loadSurface(texturePath);
                i++;
            }

        }

        (void) closedir(dp);
    }
    else
        perror ("Couldn't open the directory");

    printf("There's %d files in the current directory.\n", i);
}

void RenderScreen(){
    SDL_SetRenderDrawColor( renderer, 75, 150, 255, 100 );
    SDL_RenderClear( renderer );

    //Render texture to screen
    //renderTerrain();
    GeneratePixels();
    renderPieces();
    renderUI();
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
        uiElements[i]->RenderGameObject(renderer);
    }
}

void renderTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        /*or (int j = 0; j < terrainOutlines[i].size(); j++) {
            terrainOutlines[i][j]->RenderGameObject(renderer);
        }*/
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->RenderGameObject(renderer);
        }
    }
}

void renderPieces() {
    for (int i = 0; i < pieces.size(); i++) {
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
                    std::cout << (int)color.a << std::endl;
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
    for (int i = 0; i < pieces.size(); i++) {
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
                    return pieces[i];
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
        gameObject->SetPosition(gameObject->GetPosition().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                gameObject->GetPosition().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
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
                                piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetCenter().second);
                            } else {
                                //selectedObject = selectObject(gameObjects, playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                            }
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (selectedObject != nullptr) {
                    std::cout << selectedObject->GetName() << std::endl;
                    if (selectedObject->GetName() == "reset button") {
                        std::cout << "reset button pressed" << std::endl;
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
                            Terrain* startingTerrain = selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
                            Terrain* targetTerrain = selectTerrain(piece->GetCenter().first, piece->GetCenter().second);
                            Move(piece, startingTerrain, targetTerrain, movesLeft);

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
    int shape = rand() % (int)(15);
    int height = (rand() % rules->GetMaxHeight()) + 1;
    if (rules->GetRemainingPoints() <= rules->GetMaxHeight()) height = rules->GetRemainingPoints();
    rules->SetRemainingPoints(rules->GetRemainingPoints() - height);
    string name = to_string(height) + ", " + to_string(peaks.size() + 1);
    Peak* peak = new Peak(name, textures[to_string(shape)][height], surfaces[to_string(shape)], false, true, height);
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
        std::cout << "Odds: " << odds << std::endl;
        std::cout << "Item Chance: " << itemChance << std::endl;
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
    if (height == 1) {
        SDL_SetTextureColorMod(peak->GetTexture(), 246, 215, 176);
    } else {
        SDL_SetTextureColorMod(peak->GetTexture(), 200/rules->GetMaxHeight() * peak->GetLayer(), std::min(200/rules->GetMaxHeight() * peak->GetLayer() * 2, 255), 200/rules->GetMaxHeight() * peak->GetLayer());
    }
    GenerateTerrain(peak, shape, height);
}

void GenerateTerrain(Peak* peak, int shape, int height) {
    for (int i = height - 1; i > 0; i--) {
        string name = to_string(i) + ", " + to_string(peak->GetPeakID());
        Terrain* layer = new Terrain(name, textures[to_string(shape)][i], surfaces[to_string(shape)], false, true, i);
        layer->SetScale(peak->GetScale() + 0.1 * (height - i));
        layer->SetCenter(peak->GetCenter().first, peak->GetCenter().second);
        layer->SetRotation(peak->GetRotation());
        layer->SetPeak(peak);
        if (i == 1) {
            SDL_SetTextureColorMod(layer->GetTexture(), 246, 215, 176);
        } else {
            SDL_SetTextureColorMod(layer->GetTexture(), 200/rules->GetMaxHeight() * layer->GetLayer(), std::min(200/rules->GetMaxHeight() * layer->GetLayer() * 2, 255), 200/rules->GetMaxHeight() * layer->GetLayer());
        }
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
                    /*UIElement* circle = new UIElement("circle", textures["circle"][0], surfaces["circle"], true );
                    circle->SetResizable(true);
                    std::cout << "location of first pixel: " << (peak->GetPosition().first + i + offsetOne.first) << ", " << (peak->GetPosition().second + j + offsetOne.second) << endl;
                    std::cout << "location of second pixel: " << (other->GetPosition().first + i + offsetTwo.first) << ", " << (other->GetPosition().second + j + offsetTwo.second) << endl;
                    circle->SetCenter(peak->GetPosition().first + i + offsetOne.first, peak->GetPosition().second + j + offsetOne.second);

                    uiElements.push_back(circle);
                    gameObjects[gameObjects.size() - 1].push_back(circle);

                    std::cout << "i: " << i << std::endl;
                    std::cout << "j: " << j << std::endl;*/
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

void GroomTerrain() {
    int moveCount = 1;
    while (moveCount > 0) {
        moveCount = 0;
        for (int i = 0; i < peaks.size(); i++) {
            while (MergeTerrain(peaks[i]).size() != 0) {
                int a = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
                int b = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
                peaks[i]->SetCenter(a, b);
                peaks[i]->GetOutline()->SetCenter(a, b);
                if (peaks[i]->GetItem() != nullptr) {
                    peaks[i]->GetItem()->SetCenter(a, b);
                }
                moveCount++;
            }
            for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
                peaks[i]->childTerrain[j]->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
                peaks[i]->childTerrain[j]->GetOutline()->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
            }
        }
    }
    std::cout << "There should be no touching peaks now" << std::endl;
    ConnectTerrain();
    NeighborTerrain();


}

void ConnectTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->connectedTerrain = MergeTerrain(terrain[i][j]);
        }
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

//-------------------//

void Move(Piece* piece, Terrain* startingPoint, Terrain* targetTerrain, int& movesLeft) {

    int moveCount = 0;
    int heightDifference;
    std::vector<Terrain*> currentPath;

    bool moveSuccess = MovementAttempt(heightDifference, moveCount, startingPoint, targetTerrain, currentPath, false);

    if (!moveSuccess) {
        piece->SetCenter(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second);
    } else {
        movesLeft = movesLeft - moveCount;
        piece->SetDesignatedLocation(piece->GetCenter().first, piece->GetCenter().second);
        piece->SetOccupyingTerrain(targetTerrain);
        if (startingPoint != NULL) {
            for (int i = 0; i < startingPoint->occupants.size(); i++) {
                if (startingPoint->occupants[i] == piece) startingPoint->occupants.erase(startingPoint->occupants.begin() + i);
            }
        }
        if (targetTerrain != NULL) targetTerrain->occupants.push_back(piece);
        if (targetTerrain != NULL && targetTerrain->type == GameObject::PEAK) {
            RefreshClaimNotifs(dynamic_cast<Peak *>(targetTerrain));
        } else RefreshClaimNotifs();
    }




    UpdateMovesLeft();

    for (int i = 0; i < currentPath.size(); i++) {
        if (currentPath[i] == NULL) std::cout << "NULL" << std::endl;
        else std::cout << currentPath[i]->GetName() << std::endl;
    }
    currentPath.clear();
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

    if (AdjacentMovement(heightDifference, attemptedMoves, currentTerrain, targetTerrain, currentPath) && attemptedMoves <= movesLeft) return true;

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
        currentTurn = "playerTwo";
        currentInventory = playerTwoInventory;
    }
    else if (currentTurn == "playerTwo") {
        currentTurn = "playerThree";
        currentInventory = playerThreeInventory;
    }
    else if (currentTurn == "playerThree") {
        currentTurn = "playerFour";
        currentInventory = playerFourInventory;
    }
    else if (currentTurn == "playerFour") {
        currentTurn = "playerOne";
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
        }
    }
}

void FinishTurn() {
    std::cout << "finish turn selected" << std::endl;
    RotateTurn();
    std::cout << "Current turn: " << currentTurn << std::endl;
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
            std::cout << peak->occupants[i]->GetName() << std::endl;
            int x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
            int y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
            while (selectTerrain(x, y) != NULL) {
                x = (rand() % (int)(SCREEN_WIDTH/1.05)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.1;
                y = (rand() % (int)(SCREEN_HEIGHT/1.05)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.1;
            }
            peak->occupants[i]->SetCenter(x, y);
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
    SDL_Rect* rectangle = new SDL_Rect;
    for (int i = 0; i < SCREEN_WIDTH/8; i++) {
        for (int j = 0; j < SCREEN_HEIGHT/8; j++) {
            int x = i*8 + 4;
            int y = j*8 + 4;
            Terrain* currentTerrain = selectTerrain(x, y);
            if (currentTerrain == nullptr) continue;

            Pixel* pixel = new Pixel("pixel", textures["pixel"][0], surfaces["pixel"], false, true);
            pixel->hiddenTerrain = currentTerrain;

            y += 8;
            Terrain* nextTerrain = selectTerrain(x, y);
            int width= 8;
            int height = 8;
            rectangle->x = i*8;
            rectangle->y = j*8;
            while (nextTerrain != NULL && nextTerrain->GetLayer() == currentTerrain->GetLayer()) {
                y += 8;
                nextTerrain = selectTerrain(x, y);
                height += 8;
                j++;

            }
            rectangle->w = width;
            rectangle->h = height;


            SDL_RenderCopy(renderer, textures["pixel"][0], NULL, rectangle);

        }
    }
    delete rectangle;
}