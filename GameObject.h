#pragma once
#include <utility>
#include <string>
#include <vector>
#include "SDL.h"
using namespace std;
class GameObject {
public:
public:
    enum objectType {
        GENERIC = 0,
        TERRAIN = 1,
        PEAK = 2,
        PIECE = 3,
        UI_ELEMENT = 4
    };
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

public:
    enum objectType type = GENERIC;
    GameObject(string name, SDL_Texture* texture, SDL_Surface *surface, bool m, bool r);
    pair<float, float> GetPosition(){return position;}
    pair<float, float> GetDimensions(){return dimensions;}
    string GetName(){return name;}
    SDL_Texture* GetTexture(){return texture;}
    SDL_Surface* GetSurface(){return surface;}
    bool GetRendered() const {return rendered;}
    bool GetMovable() const {return movable;}
    float GetSize() const {return size;}
    SDL_Rect* GetRenderRect() {return renderRect;}
    pair<float, float> GetCenter() {return center;}
    float GetScale() const {return scale;}
    bool GetSelectable() const {return selectable;}
    bool GetResizable() const {return resizable;}
    void SetResizable(bool r) {resizable = r;}
    void SetTexture(SDL_Texture* t) {texture = t;}
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

class Terrain : public GameObject
{
protected:
    int layer;
    double rotation;
    Peak* peak;
    Terrain* upperTerrain = nullptr;
    Terrain* lowerTerrain = nullptr;


public:
    std::vector<Piece*> occupants;
    std::vector<Terrain*> connectedTerrain;

    void RenderGameObject(SDL_Renderer* renderer);

    Terrain(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l) : GameObject(name, texture, surface, m, r) {
        layer = l;
        scale = 1;
        type = TERRAIN;
    }
    void SetPeak(Peak* p) {peak = p;}
    Peak* GetPeak() const {return peak;}
    double GetRotation() const {return rotation;}
    Terrain* GetUpperTerrain() {return upperTerrain;}
    Terrain* GetLowerTerrain() {return lowerTerrain;}
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
    string claimedBy = "";

public:
    std::vector<Terrain*> childTerrain;
    std::vector<Piece*> flags;

    Peak(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l) : Terrain(name, texture, surface, m, r, l) {
        type = PEAK;
        peak = this;
    }
    void SetPeakID(int pid) {peakID = pid;}
    int GetPeakID() {return peakID;}
    void SetClaimNotif(UIElement* cn) {claimNotif = cn;}
    UIElement* GetClaimNotif() {return claimNotif;}
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
