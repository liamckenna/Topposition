#pragma once
#include <utility>
#include <string>
#include <vector>
#include "SDL.h"
#include <SDL2/SDL_ttf.h>
#include "GameRules.h"

#include <math.h>
#include <map>

class Animation;

using namespace std;
class GameObject {
public:
    enum objectType {
        GENERIC = 0,
        TERRAIN = 1,
        PEAK = 2,
        PIECE = 3,
        ITEM = 4,
        UI_ELEMENT = 5,
        PIXEL = 6
    };

    float GetSize() const {return size;}

protected:


    string name;
    SDL_Texture* texture;
    SDL_Surface* surface;
    bool movable = false;
    bool selectable = false;
    bool rendered = true;
    SDL_Rect* renderRect = new SDL_Rect();
    float size;
    pair<float, float> defaultPosition;
    pair<float, float> position;
    pair<int, int> dimensions;
    pair<float, float> center;
    pair<float, float> bottomRight;
    float scale = 1;
    bool resizable = true;
    Animation* currentAnimation = nullptr;

public:
    enum objectType type = GENERIC;
    GameObject(string name, SDL_Texture* texture, SDL_Surface *surface, bool m, bool r);
    pair<float, float> GetPosition(){return position;}
    pair<float, float> GetDimensions(){return dimensions;}
    string GetName(){return name;}
    SDL_Texture* GetTexture(){return texture;}
    std::map<string, Animation* > animations;
    SDL_Surface* GetSurface(){return surface;}
    bool GetRendered() const {return rendered;}
    bool GetMovable() const {return movable;}
    SDL_Rect* GetRenderRect() {return renderRect;}
    pair<float, float> GetCenter() {return center;}
    float GetScale() const {return scale;}
    bool GetSelectable() const {return selectable;}
    bool GetResizable() const {return resizable;}
    SDL_Rect* GetRectangle() const {return renderRect;}
    Animation* GetCurrentAnimation() const {return currentAnimation;}
    void SetCurrentAnimation(Animation* ca) {currentAnimation = ca;}
    void SetRectangle(SDL_Rect* r) {renderRect = r;}
    void SetResizable(bool r) {resizable = r;}
    void SetTexture(SDL_Texture* t) {texture = t;}
    void SetSurface (SDL_Surface* s) {surface = s;}
    void SetSelectable(bool s) {selectable = s;}
    void SetScale(float s) {scale = s;}
    void SetPosition(float x, float y, bool posOnly = false);
    void SetDefaultPosition(float x, float y);
    virtual void SetCenter(float x = 0, float y = 0, bool centerOnly = false);
    void SetMovable(bool m);
    void SetRendered(bool r);
    void AdjustSize(float multiplier = 1, int w = 0, int h = 0);
    virtual void RenderGameObject(SDL_Renderer* renderer);
    void SetBottomRight();
    pair<float, float> GetBottomRight() {return bottomRight;}
};

class Peak;
class Piece;
class Item;

class Terrain : public GameObject
{
protected:
    int layer;
    double rotation;
    Peak* peak;
    Terrain* upperTerrain = nullptr;
    Terrain* lowerTerrain = nullptr;
    GameObject* outline = nullptr;
    int offsetX;
    int offsetY;
    SDL_Color color;
    string biome = "";
    SDL_Texture* pixels;


public:
    std::vector<Piece*> occupants;
    std::vector<Terrain*> connectedTerrain;

    void RenderGameObject(SDL_Renderer* renderer, Terrain* hoveringTerrain);

    Terrain(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l, SDL_Renderer* renderer) : GameObject(name, texture, surface, m, r) {
        layer = l;
        scale = 1;
        type = TERRAIN;
        pixels = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET, dimensions.first, dimensions.second);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_SetTextureBlendMode(pixels, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, pixels);
        SDL_RenderClear(renderer);


        SDL_SetRenderTarget(renderer, NULL);
    }
    void SetPeak(Peak* p) {peak = p;}
    Peak* GetPeak() const {return peak;}
    SDL_Texture* GetPixels() const {return pixels;}
    double GetRotation() const {return rotation;}
    Terrain* GetUpperTerrain() {return upperTerrain;}
    Terrain* GetLowerTerrain() {return lowerTerrain;}
    GameObject* GetOutline() {return outline;}
    int GetOffsetX() {return offsetX;}
    int GetOffsetY() {return offsetY;}
    string GetBiome() {return biome;}
    SDL_Color GetColor() {return color;}
    void SetBiome(string b) {biome = b;}
    void SetColor(SDL_Color c) {color = c;}
    void SetOffsetX(int ox) {offsetX = ox;}
    void SetOffsetY(int oy) {offsetY = oy;}
    void SetOutline(GameObject* o) {outline = o;}
    void SetUpperTerrain(Terrain* ut) {upperTerrain = ut;}
    void SetLowerTerrain(Terrain* lt) {lowerTerrain = lt;}
    void SetRotation(double r) {rotation = r;}
    int GetLayer() const {return layer;}
    void SetLayer(int l) {layer = l;}


};

class UIElement : public GameObject {
    Peak* associatedPeak;
public:
    UIElement(string name, SDL_Texture *texture, SDL_Surface *surface, bool r, Peak* ap = nullptr) : GameObject(name, texture, surface, false, r) {
    type = UI_ELEMENT;
    associatedPeak = ap;
    resizable = false;
    }
    Peak* GetAssociatedPeak() {return associatedPeak;}
    void SetAssociatedPeak(Peak* ap) {associatedPeak = ap;}

};

class Peak : public Terrain {
    int peakID;
    UIElement* claimNotif;
    Item* item = nullptr;
    string claimedBy = "";

public:
    std::vector<Terrain*> childTerrain;
    std::vector<Piece*> flags;

    Peak(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l, SDL_Renderer* renderer) : Terrain(name, texture, surface, m, r, l, renderer) {
        type = PEAK;
        peak = this;
    }
    void SetPeakID(int pid) {peakID = pid;}
    int GetPeakID() {return peakID;}
    void SetClaimNotif(UIElement* cn) {claimNotif = cn;}
    UIElement* GetClaimNotif() {return claimNotif;}
    void SetItem(Item* i) {item = i;}
    Item* GetItem() {return item;}
    void Claim(string team) {
        claimedBy = team;
    }
    string GetClaimedBy() {return claimedBy;}

};

class Piece : public GameObject {
private:
    int altitude;
    pair<int, int> designatedLocation;
    Terrain* occupyingTerrain;

public:
    Piece(string name, SDL_Texture* texture, SDL_Surface* surface, bool r) : GameObject(name, texture, surface, true, r) {
    type = PIECE;
    occupyingTerrain = nullptr;
    }
    void SetDesignatedLocation(int x, int y) {
        designatedLocation.first = x;
        designatedLocation.second = y;
    }
    pair<int, int> GetDesignatedLocation() {return designatedLocation;}
    void SetAltitude(int a) {altitude = a;}
    int GetAltitude() {return altitude;}
    void SetOccupyingTerrain(Terrain* ot) {occupyingTerrain = ot;}
    Terrain* GetOccupyingTerrain() {return occupyingTerrain;}

};

class Item : public Piece {
    bool used = false;
    string owner = "";

public:
    Item(string name, SDL_Texture* texture, SDL_Surface* surface, bool r) : Piece(name, texture, surface, r) {
        type = ITEM;
        movable = false;
    }
    void SetUsed(bool u) {used = u;}
    void SetOwner(string o) {owner = o;}
    bool Getused() {return used;}
    string GetOwner() {return owner;}
};

class Pixel : public GameObject {
    Terrain* hiddenTerrain;
    SDL_Color color;
    int width;
    int height;
    bool outline = false;
public:
    Pixel(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r) : GameObject(name, texture, surface, false, r) {
        type = PIXEL;
    }
    void RenderGameObject(SDL_Renderer *renderer, Terrain* hoveringTerrain);

    void SetHiddenTerrain(Terrain* ht) {hiddenTerrain = ht;}
    void SetColor(SDL_Color c) {color = c;}
    void SetWidth(int w) {width = w;}
    void SetHeight(int h) {height = h;}
    void SetOutline(bool o) {outline = o;}

    SDL_Color GetColor() {return color;}
    bool GetOutline() {return outline;}
    Terrain* GetHiddenTerrain() {return hiddenTerrain;}

};

class Text {
    string name;
    const char* fontPath;
    TTF_Font* font;
    SDL_Color color;
    const char* text;
    std::pair<int, int> position;
    std::pair<int, int> dimensions;
    SDL_Rect* rect = new SDL_Rect();
    SDL_Surface* surface;
    SDL_Texture* texture;
    int size;
    bool rendered = true;
public:
    Text(string n, const char* fp, SDL_Color c, int x, int y, int w, int h, int s, SDL_Renderer* r, const char* t);
    void RenderText(SDL_Renderer* renderer);
    void SetRendered(bool r) {rendered = r;}

};

class Animation {
    SDL_Texture* spriteSheet;
    SDL_Surface* surface;
    SDL_Rect* rect = new SDL_Rect();
    float duration;
    int frameCount;
    pair<int, int> sheetDimensions;
    pair<int, int> spriteDimensions;
    int lastFrame = 0;
    Uint64 lastUpdate = 0;
    bool paused = false;
    int frameOffset;

public:
    Animation(SDL_Texture* ss, SDL_Surface* s, float d, int fc, pair<int, int> sd, pair<int, int> spd);
    void CycleFrame(Uint64 current);
    SDL_Rect* GetRect() {return rect;}
    SDL_Texture* GetSpriteSheet() {return spriteSheet;}
    void Pause() {paused = true;}
    void Unpause() {paused = false;}
};