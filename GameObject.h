#pragma once
#include <utility>
#include <string>
#include <vector>
#include "SDL.h"
using namespace std;
class GameObject {
    float originalRatio;
protected:
    string name;
    SDL_Texture* texture;
    SDL_Surface* surface;
    bool movable = false;
    bool rendered = true;
    SDL_Rect* renderRect = new SDL_Rect();
    float size;
    pair<float, float> defaultPosition;
    pair<float, float> position;
    pair<int, int> dimensions;
    pair<float, float> center;
    bool isPeak = false;
    pair<float, float> bottomRight;
    float scale = 1;

public:
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
    bool GetIsPeak() {return isPeak;}
    float GetScale() const {return scale;}
    void SetScale(float s) {scale = s;}
    void SetIsPeak(bool ip) {isPeak = ip;}
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
class Terrain : public GameObject
{
protected:
    int layer;
    double rotation;
    Terrain* peak;


public:
    std::vector<GameObject*> neighboringTerrain;
    std::vector<GameObject*> connectedTerrain;

    void RenderGameObject(SDL_Renderer* renderer);

    Terrain(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l) : GameObject(name, texture, surface, m, r) {
        layer = l;
        scale = 1;

    }
    void SetPeak(Terrain* p) {peak = p;}
    Terrain* GetPeak() const {return peak;}
    double GetRotation() const {return rotation;}
    void SetRotation(double r) {rotation = r;}
    int GetLayer() const {return layer;}
    void SetLayer(int l) {layer = l;}

};
class Peak : public Terrain {



public:
    std::vector<Terrain*> childTerrain;

    Peak(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l) : Terrain(name, texture, surface, m, r, l) {
    }

};

