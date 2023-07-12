#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <utility>
#include <map>
#include <vector>
#include <dirent.h>
#include <typeinfo>
#include "GameObject.h"
#include "GameRules.h"
#include "Input.h"
bool init();

bool loadMap();

void loadGamePieces();

void loadUI();

void close();

void ResetMap();

SDL_Surface* loadSurface( std::string path );

SDL_Texture* loadTexture( std::string path );

void TextureLoader();

void zoom(SDL_Event e, Input* playerInput);

void renderObjects(SDL_Renderer* gRenderer);

void renderUI();

void renderClaimNotifs();

void renderTerrain();

void renderPieces();

void renderInventory();

void renderPixels();

void renderText();

void scroll(Input* playerInput);

GameObject* selectObject(int x, int y);

UIElement* selectUI(int x, int y);

Piece* selectPiece(int x, int y);

Terrain* selectTerrain(int x, int y);

Item* selectItem(int x, int y);

SDL_Color GetPixelColor(const SDL_Surface* surface, const int X, const int Y);

void moveHeldObject(GameObject* gameObject, Input* playerInput);

void RecenterScreen(Input* playerInput);

void HandleEvents(Input* playerInput);

void RenderScreen();

void GeneratePeak();

void GenerateTerrain(Peak* peak, int shape, int height);

std::vector<Terrain*> MergeTerrain(Terrain* peak);

bool TerrainIsSurrounded(Terrain* peak, Terrain* other);

void GroomTerrain();

void ConnectTerrain();

void ConnectConnectedTerrain(std::vector<Terrain*>& connectedTerrain, Terrain* currentTerrain);

void NeighborTerrain();

void SetTerrainBiome();

int Roll();

void Move(Piece* piece, Terrain* startingPoint, Terrain* targetTerrain, int& movesLeft);

bool MovementAttempt(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain, std::vector<Terrain*>& currentPath, bool fromAdjacent);

bool DirectMovementUp(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath);

bool DirectMovementDown(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath);

Terrain* GetTargetTerrainBase(Terrain* targetTerrain);

bool AdjacentMovement(int& heightDifference, int& attemptedMoves, Terrain* currentTerrain, Terrain*& targetTerrain ,std::vector<Terrain*>& currentPath);

void RotateTurn();

void UpdateMovesLeft();

void RefreshClaimNotifs(Peak* individualPeak = NULL);

void FinishTurn();

void ClaimPeak(UIElement* claimNotif);

void PeakBattle(Peak* peak, string attackingTeam, string defendingTeam);

void Retreat(Peak* peak, string retreatingTeam);

void UpdateScore();

void GameFinished(string winner);

void Tiebreaker();

void GeneratePixels();

void AnimationHandler(int& frame, float fps, Uint64& lastFrame, Uint64& lastUpdate);

extern SDL_Window* window;
//The surface contained by the window
extern SDL_Surface* screenSurface;
//The window renderer
extern SDL_Renderer* renderer;
//Current displayed texture
extern SDL_Texture* texture;

extern int SCREEN_WIDTH;

extern int SCREEN_HEIGHT;

extern bool quit;

extern std::map<std::string, map<int,  SDL_Texture*>> textures;

extern std::map<std::string, SDL_Surface*> surfaces;

extern std::vector<std::vector<GameObject*>> gameObjects;

extern std::vector<Peak*> peaks;

extern std::vector<std::vector<Terrain*>> terrain;

extern std::vector<Pixel*> pixels;

extern std::vector<std::vector<GameObject*>> terrainOutlines;

extern std::vector<UIElement*> uiElements;

extern std::vector<Piece*> pieces;

extern std::vector<Text*> text;

extern GameObject* heldObject;

extern GameRules* rules;

extern int shapeCount;

extern int currentRoll;

extern int movesLeft;

extern int playerOneScore;

extern int playerTwoScore;

extern int playerThreeScore;

extern int playerFourScore;

extern std::vector<Item*> playerOneInventory;

extern std::vector<Item*> playerTwoInventory;

extern std::vector<Item*> playerThreeInventory;

extern std::vector<Item*> playerFourInventory;

extern std::vector<Item*> currentInventory;

extern Text* playerOneText;

extern Text* playerTwoText;

extern Text* playerThreeText;

extern Text* playerFourText;

extern Terrain* hoveringTerrain;


enum gameState {
    MAIN_MENU = 0,
    SETTINGS = 1,
    LOBBY = 2,
    GAME = 3
};
