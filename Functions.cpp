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

std::vector<UIElement*> uiElements;

std::vector<Piece*> pieces;

GameObject* selectedObject = nullptr;

GameRules* rules = new GameRules();


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

bool loadMap(std::vector<std::vector<GameObject*>> &gameObjects)
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
        //generate map layers
    }
    gameObjects[0].push_back(map);
    for (int i = 0; i < rules->GetPeakCount(); i++) {
        int x = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
        int y = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
        int shape = rand() % (int)(7);
        int height = (rand() % rules->GetMaxHeight()) + 1;
        Peak* peak = new Peak("peak", textures[to_string(shape)][height], surfaces[to_string(shape)], false, true, height);
        double rotation = rand() % 360;
        peak->SetRotation(rotation);
        peak->SetScale(0.1);
        peak->SetCenter(x, y);
        peaks.push_back(peak);
        terrain[height].push_back(peak);
        gameObjects[height].push_back(peak);

        while (MergeTerrain(peak).size() != 0) {
            int a = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
            int b = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
            peak->SetCenter(a, b);
        }
        std::cout << "peak size is " << MergeTerrain(static_cast<Terrain *>(peak)).size() << std::endl;

        SDL_SetTextureColorMod(peak->GetTexture(), 255/rules->GetMaxHeight() * peak->GetLayer(), 255/rules->GetMaxHeight() * peak->GetLayer(), 255/rules->GetMaxHeight() * peak->GetLayer());
        GenerateTerrain(peak, shape, height);
    }
    GroomTerrain();


    return success;
}

void loadGamePieces(std::vector<std::vector<GameObject*>> &gameObjects)
{
    gameObjects.push_back(vector<GameObject*>());
    GameObject* coin = new GameObject("coin", textures["coin"][0], surfaces["coin"], true, true);
    Piece* red = new Piece("red", textures["red"][0], surfaces["red"], true);

    gameObjects[rules->GetMaxHeight() + 1].push_back(coin);
    gameObjects[rules->GetMaxHeight() + 1].push_back(red);
    pieces.push_back(red);
    coin->SetCenter(960, 540);
    red->SetPosition(300, 300);
    red->SetScale((float) 1/22);
    red->SetSelectable(true);
    coin->SetSelectable(true);
}

void loadUI() {

    UIElement* resetButton = new UIElement("reset button", textures["reset"][0], surfaces["reset"], true);
    uiElements.push_back(resetButton);
    resetButton->SetPosition(0,0);
    UIElement* die1 = new UIElement("dieOne", textures["die 1"][0], surfaces["die 1"], true);
    die1->SetScale(0.1);
    uiElements.push_back(die1);
    die1->SetPosition(SCREEN_WIDTH - (die1->GetDimensions().first * die1->GetScale()) - 10, 10);
    UIElement* die2 = new UIElement("dieTwo", textures["die 2"][0], surfaces["die 2"], true);
    die2->SetScale(0.1);
    uiElements.push_back(die2);
    die2->SetPosition(SCREEN_WIDTH - ((die1->GetDimensions().first * die1->GetScale()) + 10 )*2, 10);
}

void ResetMap() {
    for (int i = 0; i < peaks.size(); i++) {
        peaks[i] = nullptr;
    }

    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j] = nullptr;
        }
    }

    for (int i = 0; i < pieces.size(); i++) {
        pieces[i] = nullptr;
    }

    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            delete gameObjects[i][j];
        }
    }
    std::vector<Piece*> newPieces;
    std::vector<std::vector<Terrain*>> newTerrain;
    std::vector<std::vector<GameObject*>> newGameObjects;
    std::vector<Peak*> newPeaks;
    pieces = newPieces;
    terrain = newTerrain;
    gameObjects = newGameObjects;
    peaks = newPeaks;
    loadMap(gameObjects);
    loadGamePieces(gameObjects);


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

void zoom(std::vector<std::vector<GameObject*>> gameObjects, SDL_Event e, Input* playerInput) {

    std::pair<float, float> relativePositionB4;
    std::pair<float, float> newRelativePosition;
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetRendered()) {
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
    RecenterScreen(gameObjects, playerInput);
}

void renderObjects(std::vector<std::vector<GameObject*>> gameObjects, SDL_Renderer* renderer) {
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
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->RenderGameObject(renderer);
        }
    }
}

void renderPieces() {
    for (int i = 0; i < pieces.size(); i++) {
        pieces[i]->RenderGameObject(renderer);
    }
}

void scroll(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
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
            if (enabled){
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + playerInput->currentMousePosition.first - playerInput->prevMousePosition.first,
                                               gameObjects[i][j]->GetPosition().second + playerInput->currentMousePosition.second - playerInput->prevMousePosition.second);
            }
        }
    }
    RecenterScreen(gameObjects, playerInput);
}

GameObject* selectObject(std::vector<std::vector<GameObject*>> gameObjects, int x, int y) {

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
                    std::cout << gameObjects[i][j]->GetName() << std::endl;
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
        int height_UpperBound = uiElements[i]->GetBottomRight().first;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(uiElements[i]->GetSurface(),
                                                (x - width_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()),
                                                (y - height_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()));
                if (color.r == 0 && color.g == 0 && color.b == 0) {

                    continue;
                } else {
                    return uiElements[i];
                }


            }
        }

    }
    return nullptr;
}

Piece* selectPiece(int x, int y) {
    for (int i = 0; i < pieces.size(); i++) {
        int width_LowerBound = pieces[i]->GetPosition().first;
        int width_UpperBound = pieces[i]->GetBottomRight().first;
        int height_LowerBound = pieces[i]->GetPosition().second;
        int height_UpperBound = pieces[i]->GetBottomRight().first;
        if (x >= width_LowerBound && x <= width_UpperBound) {
            if (y >= height_LowerBound && y <= height_UpperBound) {

                SDL_Color color = GetPixelColor(uiElements[i]->GetSurface(),
                                                (x - width_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()),
                                                (y - height_LowerBound)/(uiElements[i]->GetSize() * uiElements[i]->GetScale()));
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

                    SDL_Color color = GetPixelColor(gameObjects[i][j]->GetSurface(),
                                                    (x - width_LowerBound)/(terrain[i][j]->GetSize() * terrain[i][j]->GetScale()),
                                                    (y - height_LowerBound)/(terrain[i][j]->GetSize() * terrain[i][j]->GetScale()));

                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    }
                    std::cout << terrain[i][j]->GetName() << std::endl;
                    return terrain[i][j];

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

void RecenterScreen(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
    if (gameObjects[0][0]->GetPosition().first > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first - gameObjects[0][0]->GetPosition().first,
                                               gameObjects[i][j]->GetPosition().second);
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize() < SCREEN_WIDTH) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first + (SCREEN_WIDTH - (gameObjects[0][0]->GetPosition().first + gameObjects[0][0]->GetDimensions().first * gameObjects[0][0]->GetSize())),
                                               gameObjects[i][j]->GetPosition().second);
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second > 0) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                               gameObjects[i][j]->GetPosition().second - gameObjects[0][0]->GetPosition().second);
            }
        }
    }
    if (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize() < SCREEN_HEIGHT) {
        for (int i = gameObjects.size() - 1; i >= 0; i--) {
            for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
                gameObjects[i][j]->SetPosition(gameObjects[i][j]->GetPosition().first,
                                               gameObjects[i][j]->GetPosition().second + (SCREEN_HEIGHT - (gameObjects[0][0]->GetPosition().second + gameObjects[0][0]->GetDimensions().second * gameObjects[0][0]->GetSize())));
            }
        }
    }
}

void HandleEvents(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
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
                    zoom(gameObjects, e, playerInput);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                playerInput->MouseButtonDown(e.button);
                if (playerInput->GetMouseButtonDown("Left")) {
                    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
                    selectedObject = selectUI(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                    if (selectedObject == nullptr) {
                        selectedObject = selectPiece(playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                        if (selectedObject == nullptr) {
                            selectedObject = selectObject(gameObjects, playerInput->currentMousePosition.first, playerInput->currentMousePosition.second);
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (selectedObject != nullptr) {
                    if (selectedObject->GetName() == "reset button") {
                        std::cout << "reset button pressed" << std::endl;
                        ResetMap();
                    } else if ((selectedObject->GetName() == "dieOne" || selectedObject->GetName() == "dieTwo") && movesLeft == 0) {
                        currentRoll = Roll();
                        movesLeft = currentRoll;
                    } else if (selectedObject->type == GameObject::PIECE) {
                        Piece* piece = dynamic_cast<Piece *>(selectedObject);
                        piece->Move(selectTerrain(piece->GetDesignatedLocation().first, piece->GetDesignatedLocation().second) ,selectTerrain(piece->GetPosition().first, piece->GetPosition().second), movesLeft);
                    }
                }
                playerInput->MouseButtonUp(e.button);
                selectedObject = nullptr;
                break;
        }
    }
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    if (playerInput->GetMouseButtonDown("Middle")) {
        scroll(gameObjects, playerInput);
    }
    if (playerInput->GetMouseButtonDown("Left")) {
        if (selectedObject != nullptr) {
            moveSelectedObject(selectedObject, playerInput);
        }
    }
    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}

void RenderScreen(std::vector<std::vector<GameObject*>> gameObjects){
    SDL_SetRenderDrawColor( renderer, 0, 100, 255, 100 );
    SDL_RenderClear( renderer );

    //Render texture to screen

    renderTerrain();
    renderPieces();
    renderUI();

    //Update screen
    SDL_RenderPresent( renderer);
}

void GenerateTerrain(Peak* peak, int shape, int height) {
    for (int i = height - 1; i > 0; i--) {
        Terrain* layer = new Terrain(to_string(gameObjects[i].size()), textures[to_string(shape)][i], surfaces[to_string(shape)], false, true, i);
        layer->SetScale(peak->GetScale() + 0.1 * (height - i));
        layer->SetCenter(peak->GetCenter().first, peak->GetCenter().second);
        layer->SetRotation(peak->GetRotation());
        layer->SetPeak(peak);
        SDL_SetTextureColorMod(layer->GetTexture(), 255/rules->GetMaxHeight() * layer->GetLayer(), 255/rules->GetMaxHeight() * layer->GetLayer(), 255/rules->GetMaxHeight() * layer->GetLayer());
        gameObjects[i].push_back(layer);
        terrain[i].push_back(layer);
        peak->childTerrain.push_back(layer);

    }
}

std::vector<GameObject*> MergeTerrain(Terrain* peak) {
    std::vector<GameObject*> connectedTerrain;
    for (int k = 0; k < gameObjects[peak->GetLayer()].size(); k++) {
        if (peak == gameObjects[peak->GetLayer()][k]) continue;
        if ((peak->GetPosition().first > gameObjects[peak->GetLayer()][k]->GetPosition().first &&
             peak->GetPosition().first < gameObjects[peak->GetLayer()][k]->GetBottomRight().first &&
             peak->GetPosition().second > gameObjects[peak->GetLayer()][k]->GetPosition().second &&
             peak->GetPosition().second < gameObjects[peak->GetLayer()][k]->GetBottomRight().second) ||
            (peak->GetBottomRight().first > gameObjects[peak->GetLayer()][k]->GetPosition().first &&
             peak->GetBottomRight().first < gameObjects[peak->GetLayer()][k]->GetBottomRight().first &&
             peak->GetPosition().second > gameObjects[peak->GetLayer()][k]->GetPosition().second &&
             peak->GetPosition().second < gameObjects[peak->GetLayer()][k]->GetBottomRight().second) ||
            (peak->GetPosition().first > gameObjects[peak->GetLayer()][k]->GetPosition().first &&
             peak->GetPosition().first < gameObjects[peak->GetLayer()][k]->GetBottomRight().first &&
             peak->GetBottomRight().second > gameObjects[peak->GetLayer()][k]->GetPosition().second &&
             peak->GetBottomRight().second < gameObjects[peak->GetLayer()][k]->GetBottomRight().second) ||
            (peak->GetBottomRight().first > gameObjects[peak->GetLayer()][k]->GetPosition().first &&
             peak->GetBottomRight().first < gameObjects[peak->GetLayer()][k]->GetBottomRight().first &&
             peak->GetBottomRight().second > gameObjects[peak->GetLayer()][k]->GetPosition().second &&
             peak->GetBottomRight().second < gameObjects[peak->GetLayer()][k]->GetBottomRight().second)) {
            //std::cout << peak->GetCenter().first << ", " <<  peak->GetCenter().second << std::endl;
            //std::cout << gameObjects[peak->GetLayer()][k]->GetCenter().first << ", " <<  gameObjects[peak->GetLayer()][k]->GetCenter().second << std::endl;
            connectedTerrain.push_back(gameObjects[peak->GetLayer()][k]);
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
                moveCount++;
                std::cout << moveCount << std::endl;
                if (moveCount > 1000) break;
            }
            for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
                peaks[i]->childTerrain[j]->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
            }
        }
        if (moveCount > 1000) break;
    }

    ConnectTerrain();
    NeighborTerrain();
    std::cout << "There should be no touching peaks now" << std::endl;

}

void ConnectTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->connectedTerrain = MergeTerrain(terrain[i][j]);
        }
    }
}

void NeighborTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            std::cout << "Children count: " << terrain[i][j]->GetPeak()->childTerrain.size() << std::endl;
            std::cout << "Terrain Layer: " << terrain[i][j]->GetLayer() << std::endl;
            if (terrain[i][j]->GetLayer() != 1) {
                terrain[i][j]->neighboringTerrain.push_back(terrain[i][j]->GetPeak()->childTerrain[terrain[i][j]->GetLayer() - 1]);
            }
            if (terrain[i][j]->type != GameObject::PEAK) {
                terrain[i][j]->neighboringTerrain.push_back(terrain[i][j]->GetPeak()->childTerrain[terrain[i][j]->GetLayer() - 1]);
            }
        }
    }
}

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
    return rollOne + rollTwo;
}
