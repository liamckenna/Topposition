#pragma once
#include <utility>
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "GameRules.h"
#include "Player.h"
#include <math.h>
#include <map>
#include <iostream>

class Animation;
class Player;

enum gameState
{
    MAIN_MENU = 0,
    LOADING = 1,
    GAME = 2,
    PAUSED = 3
};

enum pauseState
{
    MAIN = 0,
    SETTINGS = 1,
    RULES = 2,
    WINNER = 3
};

enum objectType
{
    GENERIC = 0,
    TERRAIN = 1,
    PEAK = 2,
    PIECE = 3,
    ITEM = 4,
    UI_ELEMENT = 5,
    PIXEL = 6,
    OCEAN_TILE = 7,
    FLAG = 8
};

enum BattlePhase
{
    BATTLE_IDLE,
    BATTLE_START_ROUND,
    BATTLE_WAIT_ATTACK_ROLL,
    BATTLE_REVEAL_ATTACK,
    BATTLE_WAIT_DEFENSE_ROLL,
    BATTLE_WAIT_DIE_CLICKS,
    BATTLE_RESOLVE_ROUND,
    BATTLE_FATAL_ATTACK,
    BATTLE_FATAL_DEFENSE,
    BATTLE_ADVANCE,
    BATTLE_END
};

using namespace std;
class GameObject
{
protected:
    string name;
    SDL_Texture *texture;
    SDL_Surface *surface;
    bool movable = false;
    bool selectable = false;
    bool rendered = true;
    bool topLayer = false;
    SDL_FRect *renderRect = new SDL_FRect();
    pair<float, float> position;
    pair<float, float> dimensions;
    pair<float, float> center;
    pair<float, float> bottomRight;
    float scale = 1;
    bool resizable = true;
    Animation *currentAnimation = nullptr;

public:
    enum objectType type = GENERIC;
    GameObject(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r);
    pair<float, float> GetPosition(bool update = true);
    pair<float, float> GetDimensions() { return dimensions; }
    pair<float, float> GetGlobalCenter();
    string GetName() { return name; }
    SDL_Texture *GetTexture() { return texture; }
    std::map<string, Animation *> animations;
    SDL_Surface *GetSurface() { return surface; }
    bool GetRendered() const { return rendered; }
    bool GetMovable() const { return movable; }
    SDL_FRect *GetRenderRect() { return renderRect; }
    pair<float, float> GetCenter() { return center; }
    float GetScale() const { return scale; }
    bool GetSelectable() const { return selectable; }
    bool GetResizable() const { return resizable; }
    SDL_FRect *GetRectangle() const { return renderRect; }
    Animation *GetCurrentAnimation() const { return currentAnimation; }
    pair<float, float> GetBottomRight(bool update = true);
    pair<float, float> GetBottomMiddle(bool update = true);
    bool GetTopLayer() { return topLayer; }
    void SetCurrentAnimation(Animation *ca) { currentAnimation = ca; }
    void SetRectangle(SDL_FRect *r) { renderRect = r; }
    void SetResizable(bool r) { resizable = r; }
    void SetTexture(SDL_Texture *t) { texture = t; }
    void SetSurface(SDL_Surface *s) { surface = s; }
    void SetSelectable(bool s) { selectable = s; }
    void SetScale(float s) { scale = s; }
    void SetGlobalPosition(float x, float y, bool updateRelative = true);
    void SetGlobalCenter(float x, float y, bool updateRelative = true);
    void SetPosition(float x, float y, bool posOnly = false);
    virtual void SetCenter(float x = 0, float y = 0, bool centerOnly = false);
    void SetMovable(bool m);
    void SetRendered(bool r);
    virtual void RenderGameObject(SDL_Renderer *renderer);
    void SetBottomRight(float x = 0, float y = 0, bool brOnly = false);
    void SetBottomMiddle(float x = 0, float y = 0, bool bmOnly = false);
    void SetTopLayer(bool tl) { topLayer = tl; }
    void UpdateRelativePositions();
    void UpdatePosition();
    void UpdateCenter();
    void UpdateBottomRight();
    void UpdateGlobalPosition();

    pair<float, float> globalPosition;
};

class Peak;
class Piece;
class Item;

class Terrain : public GameObject
{
protected:
    int layer;
    double rotation;
    Peak *peak;
    Terrain *upperTerrain = nullptr;
    Terrain *lowerTerrain = nullptr;
    int offsetX;
    int offsetY;
    SDL_Color color;
    string biome = "";
    SDL_Texture *pixels;

public:
    std::vector<Piece *> occupants;
    std::vector<Terrain *> connectedTerrain;

    void RenderGameObject(SDL_Renderer *renderer, Terrain *hoveringTerrain, bool validHover);

    Terrain(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l, SDL_Renderer *renderer) : GameObject(name, texture, surface, m, r)
    {
        layer = l;
        scale = 1;
        type = TERRAIN;
        pixels = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1400, 1400);
        SDL_SetTextureScaleMode(pixels, SDL_SCALEMODE_NEAREST);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_SetTextureBlendMode(pixels, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, pixels);
        SDL_RenderClear(renderer);

        SDL_SetRenderTarget(renderer, NULL);
    }
    void SetPeak(Peak *p) { peak = p; }
    Peak *GetPeak() const { return peak; }
    SDL_Texture *GetPixels() const { return pixels; }
    double GetRotation() const { return rotation; }
    Terrain *GetUpperTerrain() { return upperTerrain; }
    Terrain *GetLowerTerrain() { return lowerTerrain; }
    int GetOffsetX() { return offsetX; }
    int GetOffsetY() { return offsetY; }
    string GetBiome() { return biome; }
    SDL_Color GetColor() { return color; }
    void SetBiome(string b) { biome = b; }
    void SetColor(SDL_Color c) { color = c; }
    void SetOffsetX(int ox) { offsetX = ox; }
    void SetOffsetY(int oy) { offsetY = oy; }
    void SetUpperTerrain(Terrain *ut) { upperTerrain = ut; }
    void SetLowerTerrain(Terrain *lt) { lowerTerrain = lt; }
    void SetRotation(double r) { rotation = r; }
    int GetLayer() const { return layer; }
    void SetLayer(int l) { layer = l; }
};

bool CheckMovePossibility(Piece *piece, Terrain *targetTerrain);

class UIElement : public GameObject
{
    Peak *associatedPeak;
    SDL_FRect *shadowRect;
    SDL_Texture *shadowTexture;
    bool renderShadow = false;
    int shadowOffsetX;
    int shadowOffsetY;
    Uint8 shadowAlpha;
    int rotation;
    bool selected = false;
    bool hovered = false;
    gameState gameStateContext;
    pauseState pauseStateContext = MAIN;

public:
    UIElement(string name, SDL_Texture *texture, SDL_Surface *surface, bool r, bool s, SDL_Renderer *renderer, gameState gsc, Peak *ap = nullptr) : GameObject(name, texture, surface, false, r)
    {
        type = UI_ELEMENT;
        associatedPeak = ap;
        resizable = false;
        selectable = s;
        gameStateContext = gsc;
        topLayer = true;
        rotation = 0;
        shadowOffsetX = 10;
        shadowOffsetY = 10;
        shadowAlpha = 150;
        RefreshShadow(renderer);
    }

    void RefreshShadow(SDL_Renderer *renderer) {
        
        shadowRect = new SDL_FRect();
        SDL_Surface *shadowSurface = SDL_DuplicateSurface(surface);
        shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
        SDL_SetTextureScaleMode(shadowTexture, SDL_SCALEMODE_NEAREST);
        SDL_DestroySurface(shadowSurface);

        SDL_SetTextureColorMod(shadowTexture, 0, 0, 0);
        SDL_SetTextureAlphaMod(shadowTexture, shadowAlpha / 2);
        SDL_SetTextureBlendMode(shadowTexture, SDL_BLENDMODE_BLEND);
    }
    
    void SetAssociatedPeak(Peak *ap) { associatedPeak = ap; }
    void SetRenderShadow(bool rs) { renderShadow = rs; }
    void SetRotation(int r) { rotation = r; }
    void SetSelected(bool s) { selected = s; }
    void SetHovered(bool h) { hovered = h; }
    void SetGameStateContext(gameState gsc) { gameStateContext = gsc; }
    void SetShadowOffset(int x, int y) { shadowOffsetX = x; shadowOffsetY = y; }
    void SetPauseStateContextP(pauseState psc) { pauseStateContext = psc; }
    gameState GetGameStateContext() { return gameStateContext; }
    pauseState GetPauseStateContext() { return pauseStateContext; }
    Peak *GetAssociatedPeak() { return associatedPeak; }
    bool GetHovered() { return hovered; }
    bool GetSelected() { return selected; }
    int GetRotation() { return rotation; }
    bool GetRenderShadow() { return renderShadow; }
    SDL_FRect *GetShadowRect() { return shadowRect; }
    SDL_Texture *GetShadowTexture() { return shadowTexture; }
    int GetShadowOffsetX() { return shadowOffsetX; }
    int GetShadowOffsetY() { return shadowOffsetY; }
    Uint8 GetShadowAlpha() { return shadowAlpha; }
};

class Peak : public Terrain
{
    int peakID;
    UIElement *claimNotif;
    Item *item = nullptr;
    Player *claimedBy = nullptr;

public:
    std::vector<Terrain *> childTerrain;
    std::vector<Piece *> flags;

    Peak(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l, SDL_Renderer *renderer) : Terrain(name, texture, surface, m, r, l, renderer)
    {
        type = PEAK;
        peak = this;
    }
    void SetPeakID(int pid) { peakID = pid; }
    int GetPeakID() { return peakID; }
    void SetClaimNotif(UIElement *cn) { claimNotif = cn; }
    UIElement *GetClaimNotif() { return claimNotif; }
    void SetItem(Item *i) { item = i; }
    Item *GetItem() { return item; }
    void Claim(Player *player);
    Player *GetClaimedBy() { return claimedBy; }
};

class Piece : public GameObject
{
private:
    int altitude;
    pair<int, int> designatedLocation;
    Terrain *occupyingTerrain;
    Player *player;
    bool dead = false;
    bool held = false;

public:
    Piece(string name, SDL_Texture *texture, SDL_Surface *surface, bool r) : GameObject(name, texture, surface, true, r)
    {
        type = PIECE;
        occupyingTerrain = nullptr;
    }
    void SetDesignatedLocation(int x, int y)
    {
        designatedLocation.first = x;
        designatedLocation.second = y;
    }
    void SetAltitude(int a) { altitude = a; }
    void SetPlayer(Player *p) { player = p; }
    void SetOccupyingTerrain(Terrain *ot) { occupyingTerrain = ot; }
    void SetDead(bool d) { dead = d; }
    void SetHeld(bool h) { held = h; }
    pair<int, int> GetDesignatedLocation() { return designatedLocation; }
    int GetAltitude() { return altitude; }
    Player *GetPlayer() { return player; }
    Terrain *GetOccupyingTerrain() { return occupyingTerrain; }
    bool IsDead() { return dead; }
    bool IsHeld() { return held; }
};

class Item : public Piece
{
    bool used = false;
    Player *owner = nullptr;

public:
    Item(string name, SDL_Texture *texture, SDL_Surface *surface, bool r) : Piece(name, texture, surface, r)
    {
        type = ITEM;
        movable = false;
    }
    void SetUsed(bool u) { used = u; }
    void SetOwner(Player *o) { owner = o; }
    bool Getused() { return used; }
    Player *GetOwner() { return owner; }
};

class Pixel : public GameObject
{
    Terrain *hiddenTerrain;
    SDL_Color color;
    int width;
    int height;

public:
    Pixel(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r) : GameObject(name, texture, surface, false, r)
    {
        type = PIXEL;
    }
    void RenderGameObject(SDL_Renderer *renderer, Terrain *hoveringTerrain);

    void SetHiddenTerrain(Terrain *ht) { hiddenTerrain = ht; }
    void SetColor(SDL_Color c) { color = c; }
    void SetWidth(int w) { width = w; }
    void SetHeight(int h) { height = h; }

    SDL_Color GetColor() { return color; }
    Terrain *GetHiddenTerrain() { return hiddenTerrain; }
};

class OceanTile : public GameObject
{
    float lifespan;
    float tileTime;
    bool fadingOut = false;
    bool fadingIn = false;
    bool waitingToReturn = false;
    float alpha_float = 1.f;
    int rotation;

public:
    OceanTile(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r) : GameObject(name, texture, surface, m, r)
    {
        type = OCEAN_TILE;
        lifespan = rand() % 15 + 5;
        tileTime = 0;
        rotation = rand() % 360;
    }

    void CheckTimer();
    void FadeOut();
    void FullFade();
    void FadeIn();

    float GetAlphaFloat() const { return alpha_float; }
    int GetRotation() const { return rotation; }
};

class Text
{
    bool selectable = false;
    bool hovered = false;
    bool selected = false;
    string name;
    const char *fontPath;
    TTF_Font *font;
    SDL_Color color;
    std::string text;
    std::pair<int, int> position;
    std::pair<int, int> dimensions;
    SDL_FRect *rect = new SDL_FRect();
    SDL_Surface *surface;
    SDL_Texture *texture;
    int size;
    bool rendered = true;
    SDL_FRect *shadowRect = new SDL_FRect();
    SDL_Texture *shadowTexture;
    bool renderShadow = true;
    int shadowOffsetX;
    int shadowOffsetY;
    Uint8 shadowAlpha;
    gameState gameStateContext = GAME;
    pauseState pauseStateContext = MAIN;

public:
    Text(string n, const char *fp, SDL_Color c, int x, int y, int s, SDL_Renderer *r, const char *t);
    void RenderText(SDL_Renderer *renderer);
    void SetRendered(bool r) { rendered = r; }
    void SetRenderShadow(bool rs) { renderShadow = rs; }
    void SetSelectable(bool s) { selectable = s; }
    void SetGameStateContext(gameState gsc) { gameStateContext = gsc; }
    void SetPauseStateContext(pauseState psc) { pauseStateContext = psc; }
    void SetPosition(int x, int y)
    {
        position.first = x;
        position.second = y;
        rect->x = x;
        rect->y = y;
    }
    void SetCenter(int x, int y)
    {
        position.first = x - (dimensions.first / 2);
        position.second = y - (dimensions.second / 2);
        rect->x = position.first;
        rect->y = position.second;
    }
    std::pair<float, float> GetCenter()
    {
        return {position.first + (dimensions.first / 2), position.second + (dimensions.second / 2)};
    }
    void SetTextContent(const char *t, SDL_Renderer *renderer)
    {
        text = t;
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(shadowTexture);


        SDL_Color shadowColor = {0, 0, 0, shadowAlpha};
        SDL_Surface *shadowSurface = TTF_RenderText_Solid(font, text.c_str(), text.length(), shadowColor);
        shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
        SDL_SetTextureAlphaMod(shadowTexture, shadowAlpha);
        SDL_DestroySurface(shadowSurface);


        surface = TTF_RenderText_Solid(font, text.c_str(), text.length(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        dimensions.first = surface->w;
        dimensions.second = surface->h;

    }
    void SetDimensions(int w, int h)
    {
        dimensions.first = w;
        dimensions.second = h;
        rect->w = w;
        rect->h = h;
    }
    void SetSize(int s, SDL_Renderer *renderer)
    {
        size = s;
        font = TTF_OpenFont(fontPath, size);
        SetTextContent(text.c_str(), renderer);
    }
    void SetColor(SDL_Color c, SDL_Renderer *r);
    void SetHovered(bool h) { hovered = h; }
    void SetSelected(bool s) { selected = s; }
    void SetShadowOffset(int x, int y) { shadowOffsetX = x; shadowOffsetY = y; }
    int GetSize() { return size; }
    int GetWidth() { return dimensions.first; }
    int GetHeight() { return dimensions.second; }
    bool GetSelectable() { return selectable; }
    bool GetRendered() { return rendered; }
    bool GetHovered() { return hovered; }
    bool GetSelected() { return selected; }
    std::pair<int, int> GetPosition() { return position; }
    std::pair<int, int> GetDimensions() { return dimensions; }
    string GetName() { return name; }
    SDL_Surface* GetSurface() { return surface; }
    SDL_Texture* GetTexture() { return texture; }
    std::pair<int, int> GetBottomRight() { return {position.first + dimensions.first, position.second + dimensions.second}; }
    gameState GetGameStateContext() { return gameStateContext; }
    pauseState GetPauseStateContext() { return pauseStateContext; }
    string GetTextContent() { return text; }
};

class Animation
{
    SDL_Texture *spriteSheet;
    SDL_Surface *surface;
    SDL_FRect *rect = new SDL_FRect();
    float duration;
    int frameCount;
    pair<int, int> sheetDimensions;
    pair<int, int> spriteDimensions;
    int lastFrame = 0;
    Uint64 lastUpdate = 0;
    bool paused = false;
    int frameOffset;

public:
    Animation(SDL_Texture *ss, SDL_Surface *s, float d, int fc, pair<int, int> sd, pair<int, int> spd);
    void CycleFrame(Uint64 current);
    SDL_FRect *GetRect() { return rect; }
    SDL_Texture *GetSpriteSheet() { return spriteSheet; }
    void Pause() { paused = true; }
    void Unpause() { paused = false; }
    void SetLastUpdate(Uint64 lu) { lastUpdate = lu; }
    Uint64 GetLastUpdate() { return lastUpdate; }
};

struct DiceAnimation
{
    UIElement *die;
    vector<SDL_Texture *> faces;
    SDL_Texture *finalTexture;
    Uint64 startTime;
    Uint64 elapsed;
    Uint64 lastTime;
    Uint64 stepInterval;
    int currentFace = 0;
    int totalSteps;
    int currentStep;
    bool finished;
    bool revealMovesLeftOnFinish;

    DiceAnimation(UIElement *d, vector<SDL_Texture *> f, SDL_Texture *ft, Uint64 st, Uint64 si = 100, int ts = 10, bool showMoves = true)
        : die(d), faces(f), finalTexture(ft), startTime(st), lastTime(st), elapsed(0),stepInterval(si), totalSteps(ts), currentStep(0), finished(false), revealMovesLeftOnFinish(showMoves) {}
};

struct BattleSequenceState
{
    bool active = false;
    Peak *peak = nullptr;
    Player *attacker = nullptr;
    Player *defender = nullptr;
    std::vector<Piece *> attackers;
    std::vector<Piece *> defenders;
    Piece *roundAttacker = nullptr;
    Piece *roundDefender = nullptr;
    int attackRoll = 0;
    int defenseRoll = 0;
    bool attackRollStarted = false;
    bool defenseRollStarted = false;
    Uint64 phaseStart = SDL_GetTicks();
    BattlePhase phase = BATTLE_IDLE;
};