#include "ProgramInitialization.h"

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
        window = GPU_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

            //Initialize renderer color

            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;

            if( !( IMG_Init( imgFlags ) & imgFlags ) )
            {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }
            else {
                TextureLoader();
                TTF_Init();
            }
        }
    }

    return success;
}

void close()
{
    //Free loaded image
    /*for (auto i : textures) {
        SDL_DestroyTexture(i.second);
        i.second = NULL;
    }*/
    //Destroy window

    GPU_FreeTarget(window);
    window = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface( std::string path )
{
    //The final optimized image

    //Load image at specified path
    SDL_Surface* loadedSurface = GPU_LoadSurface(path.c_str());

    return loadedSurface;
}

GPU_Image* loadTexture( std::string path )
{
    //The final texture
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    GPU_Image* loadedImage = GPU_LoadImage(path.c_str());

    return loadedImage;
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
