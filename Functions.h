#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <utility>
#include <map>
#include <vector>
#include <dirent.h>
#include <typeinfo>
#include "GameObject.h"
#include "GameRules.h"
#include "Input.h"
bool init();

bool loadMap(std::vector<std::vector<GameObject*>> &gameObjects);

void loadGamePieces(std::vector<std::vector<GameObject*>> &gameObjects);

void loadUI();

void close();

SDL_Surface* loadSurface( std::string path );

SDL_Texture* loadTexture( std::string path );

void TextureLoader();

void zoom(std::vector<std::vector<GameObject*>> gameObjects, SDL_Event e, Input* playerInput);

void renderObjects(std::vector<std::vector<GameObject*>> gameObjects, SDL_Renderer* gRenderer);

void renderUI();

void renderTerrain();

void renderPieces();

void scroll(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

GameObject* selectObject(std::vector<std::vector<GameObject*>> gameObjects, int x, int y);

UIElement* selectUI(int x, int y);

Piece* selectPiece(int x, int y);

Terrain* selectTerrain(int x, int y);

SDL_Color GetPixelColor(const SDL_Surface* surface, const int X, const int Y);

void moveHeldObject(GameObject* gameObject, Input* playerInput);

void RecenterScreen(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

void HandleEvents(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

void RenderScreen(std::vector<std::vector<GameObject*>> gameObjects);

void GeneratePeak();

void GenerateTerrain(Peak* peak, int shape, int height);

std::vector<Terrain*> MergeTerrain(Terrain* peak);

void GroomTerrain();

void ConnectTerrain();

void ConnectAllTerrain(Terrain* originalTerrain, Terrain* currentTerrain);

void NeighborTerrain();

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

void UpdateScore(Peak* peak = nullptr);

void GameFinished(string winner);

void Tiebreaker();

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

extern std::vector<UIElement*> uiElements;

extern std::vector<Piece*> pieces;

extern GameObject* heldObject;

extern GameRules* rules;

extern int currentRoll;

extern int movesLeft;

extern int playerOneScore;

extern int playerTwoScore;

extern int playerThreeScore;

extern int playerFourScore;

enum gameState {
    MAIN_MENU = 0,
    SETTINGS = 1,
    LOBBY = 2,
    GAME = 3
};
