#include "GlobalVars.h"

SDL_Window *window = NULL;

SDL_Surface *screenSurface = NULL;

std::atomic<SDL_Renderer *> renderer = NULL;

SDL_Texture *texture = NULL;

int SCREEN_WIDTH = 1920;

int SCREEN_HEIGHT = 1080;

int MAP_WIDTH = 2560;

int MAP_HEIGHT = 1440;

bool quit = false;

int currentRoll = 3;

int movesLeft = 3;

gameState state = MAIN_MENU;

std::map<std::string, std::map<int, SDL_Texture *>> textures;

std::map<std::string, SDL_Surface *> surfaces;

std::vector<std::vector<GameObject *>> gameObjects;

std::vector<Peak *> peaks;

std::vector<std::vector<Terrain *>> terrain;

std::vector<Pixel *> pixels;

std::vector<UIElement *> uiElements;

std::vector<Piece *> pieces;

std::vector<std::vector<OceanTile *>> ocean;

std::vector<Text *> text;

std::vector<Animation *> animations;

std::vector<Player *> players;

GameObject *selectedObject = nullptr;

Item *selectedItem = nullptr;

GameRules *rules = new GameRules();

int shapeCount = 0;

Text *turnTallyText;

Text *turnTallyNumText;

Text *peaksLeftText;

Text *movesLeftText;

Player *currentTurn;

UIElement *currentPlayerCircle;

Terrain *startingTerrain;

Terrain *hoveringTerrain;

bool validMove = true;

Player *firstPlace;

Uint64 currentTime = 0;

Uint64 prevTime = 0;

Uint64 deltaTime = 1;

Uint64 gameStartTime = 0;

Uint64 frameCountTime = 0;

int frameCounter = 0;

float cameraZoom = 1.f;

std::pair<float, float> cameraPosition = {0.f, 0.f};

bool seaHover = false;

std::pair<float, float> worldResolution = std::make_pair(SCREEN_WIDTH, SCREEN_HEIGHT);

int turnCount = 1;

int unclaimedPeakCount;