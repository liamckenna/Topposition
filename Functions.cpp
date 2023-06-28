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

gameState currentState = MAIN_MENU;

std::map<std::string, std::map<int, SDL_Texture*>> textures;

std::map<std::string, SDL_Surface*> surfaces;

std::vector<std::vector<GameObject*>> gameObjects;

std::vector<Peak*> peaks;

GameObject* heldObject = nullptr;

GameRules* rules = new GameRules();

bool init()
{
    //Initialization flag
    bool success = true;



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
    srand((unsigned) time(NULL));

    //Load splash image
    GameObject* map = new GameObject("map", textures["map"][0], surfaces["map"], false, true);
    gameObjects.push_back(vector<GameObject*>());
    gameObjects[0].push_back(map);
    SDL_SetTextureColorMod(map->GetTexture(), 0, 0, 0);
    for (int i = 0; i < rules->GetMaxHeight() + 1; i++) {
        gameObjects.push_back(vector<GameObject *>());
        //generate map layers
    }
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
        peak->SetIsPeak(true);
        peaks.push_back(peak);
        gameObjects[height].push_back(peak);

        while (MergeTerrain(peak).size() != 0) {
            int a = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
            int b = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
            peak->SetCenter(a, b);
        }
        std::cout << "peak size is " << MergeTerrain(peak).size() << std::endl;

        SDL_SetTextureColorMod(peak->GetTexture(), 255/rules->GetMaxHeight() * peak->GetLayer(), 255/rules->GetMaxHeight() * peak->GetLayer(), 255/rules->GetMaxHeight() * peak->GetLayer());
        GenerateTerrain(peak, shape, height);
    }
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
                if (moveCount > 5000) break;
            }
            for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
                peaks[i]->childTerrain[j]->SetCenter(peaks[i]->GetCenter().first, peaks[i]->GetCenter().second);
            }
        }
    }
    std::cout << "There should be no touching peaks now" << std::endl;

    map->SetPosition(0, 0);

    return success;
}

void loadGamePieces(std::vector<std::vector<GameObject*>> &gameObjects)
{
    gameObjects.push_back(vector<GameObject*>());
    GameObject* coin = new GameObject("coin", textures["coin"][0], surfaces["coin"], true, true);
    GameObject* red = new GameObject("red", textures["red"][0], surfaces["red"], true, true);

    gameObjects[rules->GetMaxHeight() + 1].push_back(coin);
    gameObjects[rules->GetMaxHeight() + 1].push_back(red);
    coin->SetCenter(960, 540);
    red->SetPosition(300, 300);
    red->SetScale((float) 1/22);
}

void loadUI() {
    gameObjects.push_back(vector<GameObject *>());
    GameObject* resetButton = new GameObject("reset button", textures["reset"][0], surfaces["reset"], false, true);
    gameObjects[gameObjects.size() - 1].push_back(resetButton);
    resetButton->SetPosition(0,0);
}

void ResetMap() {
    for (int i = 0; i < peaks.size(); i++) {
        peaks[i] = nullptr;
    }

    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            delete gameObjects[i][j];
        }
    }

    std::vector<std::vector<GameObject*>> newGameObjects;
    std::vector<Peak*> newPeaks;
    gameObjects = newGameObjects;
    peaks = newPeaks;
    loadMap(gameObjects);
    loadGamePieces(gameObjects);
    loadUI();

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

GameObject* selectObject(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput) {
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    for (int i = gameObjects.size() - 1; i >= 0; i--) {
        for (int j = gameObjects[i].size() - 1; j >= 0; j--) {
            int width_LowerBound = gameObjects[i][j]->GetPosition().first;
            int width_UpperBound = gameObjects[i][j]->GetPosition().first + (gameObjects[i][j]->GetDimensions().first * gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale());
            int height_LowerBound = gameObjects[i][j]->GetPosition().second;
            int height_UpperBound = gameObjects[i][j]->GetPosition().second + (gameObjects[i][j]->GetDimensions().second * gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale());

            if (playerInput->currentMousePosition.first >= width_LowerBound && playerInput->currentMousePosition.first <= width_UpperBound) {
                if (playerInput->currentMousePosition.second >= height_LowerBound && playerInput->currentMousePosition.second <= height_UpperBound) {

                    SDL_Color color = GetPixelColor(gameObjects[i][j]->GetSurface(),
                                                    (playerInput->currentMousePosition.first - width_LowerBound)/(gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale()),
                                                    (playerInput->currentMousePosition.second - height_LowerBound)/(gameObjects[i][j]->GetSize() * gameObjects[i][j]->GetScale()));
                    if (color.r == 0 && color.g == 0 && color.b == 0) {

                        continue;

                    } else {
                        std::cout << gameObjects[i][j]->GetName() << std::endl;
                        if (gameObjects[i][j]->GetMovable() || gameObjects[i][j]->GetName() == "reset button"){
                            return gameObjects[i][j];
                        }
                    }


                }
            }

        }
    }
    return nullptr;
}

SDL_Color GetPixelColor(const SDL_Surface* pSurface, const int X, const int Y)
{
    // Bytes per pixel
    const Uint8 Bpp = pSurface->format->BytesPerPixel;

    /*
    Retrieve the address to a specific pixel
    pSurface->pixels	= an array containing the SDL_Surface' pixels
    pSurface->pitch		= the length of a row of pixels (in bytes)
    X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
    */
    Uint8* pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;

    Uint32 PixelData = *(Uint32*)pPixel;

    SDL_Color Color = {0x00, 0x00, 0x00};

    // Retrieve the RGB values of the specific pixel
    SDL_GetRGBA(PixelData, pSurface->format, &Color.r, &Color.g, &Color.b, &Color.a);
    return Color;
}

void moveHeldObject(GameObject* gameObject, Input* playerInput) {
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
                    heldObject = selectObject(gameObjects, playerInput);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (heldObject != NULL && heldObject->GetName() == "reset button" && heldObject == selectObject(gameObjects, playerInput)) {
                    std::cout << "reset button pressed" << std::endl;
                    ResetMap();
                }
                playerInput->MouseButtonUp(e.button);
                heldObject = nullptr;
                break;
        }
    }
    SDL_GetMouseState(&playerInput->currentMousePosition.first, &playerInput->currentMousePosition.second);
    if (playerInput->GetMouseButtonDown("Middle")) {
        scroll(gameObjects, playerInput);
    }
    if (playerInput->GetMouseButtonDown("Left")) {
        if (heldObject != nullptr) {
            moveHeldObject(heldObject, playerInput);
        }
    }
    SDL_GetMouseState(&playerInput->prevMousePosition.first, &playerInput->prevMousePosition.second);
}

void RenderScreen(std::vector<std::vector<GameObject*>> gameObjects){
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 100 );
    SDL_RenderClear( renderer );

    //Render texture to screen
    renderObjects(gameObjects, renderer);
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0x00, 0xFF );

    //Update screen
    SDL_RenderPresent( renderer);
}

void GenerateTerrain(Peak* peak, int shape, int height) {
    for (int i = height; i > 0; i--) {
        Terrain* layer = new Terrain(to_string(gameObjects[i].size()), textures[to_string(shape)][i], surfaces[to_string(shape)], false, true, i);
        layer->SetScale(peak->GetScale() + 0.1 * (height - i));
        layer->SetCenter(peak->GetCenter().first, peak->GetCenter().second);
        layer->SetRotation(peak->GetRotation());
        layer->SetPeak(peak);
        SDL_SetTextureColorMod(layer->GetTexture(), 255/rules->GetMaxHeight() * layer->GetLayer(), 255/rules->GetMaxHeight() * layer->GetLayer(), 255/rules->GetMaxHeight() * layer->GetLayer());
        gameObjects[i].push_back(layer);
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