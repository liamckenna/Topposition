#include "GlobalVars.h"

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

std::vector<Player*> players;

GameObject* selectedObject = nullptr;

Item* selectedItem = nullptr;

GameRules* rules = new GameRules();

int shapeCount = 0;

Player* currentTurn;

Text* playerOneText;

Text* playerTwoText;

Text* playerThreeText;

Text* playerFourText;

Terrain* hoveringTerrain;


