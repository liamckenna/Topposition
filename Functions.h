#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <utility>
#include <map>
#include <vector>
#include <dirent.h>
#include "GameObject.h"
#include "GameRules.h"
#include "Input.h"
bool init();

bool loadMap(std::vector<std::vector<GameObject*>> &gameObjects);

void loadGamePieces(std::vector<std::vector<GameObject*>> &gameObjects);

void close();

SDL_Surface* loadSurface( std::string path );

SDL_Texture* loadTexture( std::string path );

void TextureLoader();

void zoom(std::vector<std::vector<GameObject*>> gameObjects, SDL_Event e, Input* playerInput);

void renderObjects(std::vector<std::vector<GameObject*>> gameObjects, SDL_Renderer* gRenderer);

void scroll(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

GameObject* selectObject(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

SDL_Color GetPixelColor(const SDL_Surface* pSurface, const int X, const int Y);

void moveHeldObject(GameObject* gameObject, Input* playerInput);

void RecenterScreen(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

void HandleEvents(std::vector<std::vector<GameObject*>> gameObjects, Input* playerInput);

void RenderScreen(std::vector<std::vector<GameObject*>> gameObjects);

void GenerateTerrain(Terrain* peak, int shape);

void GenerateBase();


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

extern GameObject* heldObject;

extern GameRules* rules;

enum gameState {
    MAIN_MENU = 0,
    SETTINGS = 1,
    GAME = 2,
};