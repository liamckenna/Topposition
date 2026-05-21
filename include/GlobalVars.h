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
#include <atomic>
#include "GameObject.h"

extern SDL_Window *window;

extern SDL_Surface *screenSurface;

extern std::atomic<SDL_Renderer *> renderer;

extern SDL_Texture *texture;

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

extern std::vector<DiceAnimation> diceAnimations;

extern std::vector<Player *> players;

extern GameObject *selectedObject;

extern gameState state;

extern pauseState pState;

extern GameRules *rules;

extern Player *currentTurn;

extern Player *firstPlace;

extern BattleSequenceState* battleSequence;

extern std::pair<float, float> worldResolution;

extern std::pair<float, float> cameraPosition;

extern bool quit;

extern bool seaHover;

extern bool hovering;

extern bool validMove;

extern bool hasRolled;

extern bool allPeaksClaimed;

extern bool suddenDeath;

extern bool lastTurn;

extern float cameraZoom;

extern int SCREEN_WIDTH;

extern int SCREEN_HEIGHT;

extern int MAP_WIDTH;

extern int MAP_HEIGHT;

extern int shapeCount;

extern int currentRoll;

extern int movesLeft;

extern int turnCount;

extern int unclaimedPeakCount;

extern int frameCounter;

extern Uint64 currentTime;

extern Uint64 prevTime;

extern Uint64 deltaTime;

extern Uint64 gameStartTime;

extern Uint64 frameCountTime;

extern Terrain *startingTerrain;

extern Terrain *hoveringTerrain;

extern UIElement *currentPlayerCircle;

extern UIElement *opposingPlayerCircle;

extern UIElement *endTurnArrow;

extern UIElement *crown;

extern UIElement *die1;

extern UIElement *die2;

extern UIElement *fatalAttackExclamation;

extern UIElement *fatalDefenseExclamation;

extern Text *turnText;

extern Text *endText;

extern Text *selectedText;

extern Text *turnTallyText;

extern Text *turnTallyNumText;

extern Text *peaksLeftText;

extern Text *peaksLeftNumText;

extern Text *movesLeftText;

extern Text *pausedText;

extern Text *resetMapText;

extern Text *mainExitToMainMenuText;

extern Text *playerWinsText;

extern Text *playAgainText;

extern Text *winnerExitToMainMenuText;

extern Text *suddenDeathText;

extern Text *lastTurnText;
