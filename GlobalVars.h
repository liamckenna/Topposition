#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <utility>
#include <map>
#include <vector>
#include <dirent.h>
#include <typeinfo>
#include <thread>
#include "GameObject.h"
#include <atomic>

extern SDL_Window *window;
// The surface contained by the window
extern SDL_Surface *screenSurface;
// The window renderer
extern std::atomic<SDL_Renderer *> renderer;
// Current displayed texture
extern SDL_Texture *texture;

extern int SCREEN_WIDTH;

extern int SCREEN_HEIGHT;

extern bool quit;

extern std::map<std::string, std::map<int, SDL_Texture *>> textures;

extern std::map<std::string, SDL_Surface *> surfaces;

extern std::vector<std::vector<GameObject *>> gameObjects;

extern std::vector<Peak *> peaks;

extern std::vector<std::vector<Terrain *>> terrain;

extern std::vector<std::vector<Pixel *>> pixels;

extern std::vector<std::vector<GameObject *>> terrainOutlines;

extern std::vector<UIElement *> uiElements;

extern std::vector<Piece *> pieces;

extern std::vector<Text *> text;

extern std::vector<Animation *> animations;

extern std::vector<Player *> players;

extern GameObject *selectedObject;

extern GameRules *rules;

extern int shapeCount;

extern int currentRoll;

extern int movesLeft;

extern Text *playerOneText;

extern Text *playerTwoText;

extern Text *playerThreeText;

extern Text *playerFourText;

extern Player *currentTurn;

extern Terrain *hoveringTerrain;

extern bool validMove;

enum gameState
{
    MAIN_MENU = 0,
    SETTINGS = 1,
    LOBBY = 2,
    GAME = 3
};

extern gameState state;

extern Player *first_place;