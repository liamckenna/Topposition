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

extern int MAP_WIDTH;

extern int MAP_HEIGHT;

extern bool quit;

extern std::map<std::string, std::map<int, SDL_Texture *>> textures;

extern std::map<std::string, SDL_Surface *> surfaces;

extern std::vector<std::vector<GameObject *>> gameObjects;

extern std::vector<Peak *> peaks;

extern std::vector<std::vector<Terrain *>> terrain;

extern std::vector<Pixel *> pixels;

extern std::vector<UIElement *> uiElements;

extern std::vector<Piece *> pieces;

extern std::vector<Text *> text;

extern std::vector<std::vector<OceanTile *>> ocean;

extern std::vector<Animation *> animations;

extern std::vector<Player *> players;

extern GameObject *selectedObject;

extern Text *selectedText;

extern GameRules *rules;

extern int shapeCount;

extern int currentRoll;

extern int movesLeft;

extern Text *turnTallyText;

extern Text *turnTallyNumText;

extern Text *peaksLeftText;

extern Text *movesLeftText;

extern Player *currentTurn;

extern UIElement *currentPlayerCircle;

extern Terrain *startingTerrain;

extern Terrain *hoveringTerrain;

extern bool validMove;

extern Uint64 currentTime;

extern Uint64 prevTime;

extern Uint64 deltaTime;

extern Uint64 gameStartTime;

extern Uint64 frameCountTime;

extern int frameCounter;

enum gameState
{
    MAIN_MENU = 0,
    SETTINGS = 1,
    LOADING = 2,
    GAME = 3,
    PAUSED = 4
};

extern gameState state;

extern Player *firstPlace;

extern float cameraZoom;

extern std::pair<float, float> cameraPosition;

extern bool seaHover;

extern bool hovering;

extern std::pair<float, float> worldResolution;

extern int turnCount;

extern int unclaimedPeakCount;