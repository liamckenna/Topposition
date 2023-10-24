#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_gpu.h>
#include <SDL2/SDL_ttf.h>
#include <utility>
#include <map>
#include <vector>
#include <dirent.h>
#include <typeinfo>
#include "GameObject.h"


extern GPU_Target* window;
//The surface contained by the window
extern SDL_Surface* screenSurface;
//Current displayed texture
extern SDL_Texture* texture;

extern int SCREEN_WIDTH;

extern int SCREEN_HEIGHT;

extern bool quit;

extern std::map<std::string, map<int,  GPU_Image*>> textures;

extern std::map<std::string, SDL_Surface*> surfaces;

extern std::vector<std::vector<GameObject*>> gameObjects;

extern std::vector<Peak*> peaks;

extern std::vector<std::vector<Terrain*>> terrain;

extern std::vector<Pixel*> pixels;

extern std::vector<std::vector<GameObject*>> terrainOutlines;

extern std::vector<UIElement*> uiElements;

extern std::vector<Piece*> pieces;

extern std::vector<Text*> text;

extern std::vector<Animation*> animations;

extern std::vector<Player*> players;

extern GameObject* selectedObject;

extern GameRules* rules;

extern int shapeCount;

extern int currentRoll;

extern int movesLeft;

extern Text* playerOneText;

extern Text* playerTwoText;

extern Text* playerThreeText;

extern Text* playerFourText;

extern Player* currentTurn;

extern Terrain* hoveringTerrain;


enum gameState {
    MAIN_MENU = 0,
    SETTINGS = 1,
    LOBBY = 2,
    GAME = 3
};
