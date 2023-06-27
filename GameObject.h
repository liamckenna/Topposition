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

public:
    GameObject(string name, SDL_Texture* texture, SDL_Surface *surface, bool m, bool r);
    pair<float, float> GetPosition(){return position;}
    pair<float, float> GetDimensions(){return dimensions;}
    string GetName(){return name;}
    SDL_Texture* GetTexture(){return texture;}
    SDL_Surface* GetSurface(){return surface;}
    bool GetRendered() {return rendered;}
    bool GetMovable() {return movable;}
    float GetSize() {return size;}
    virtual float GetScale() {return 1;}
    SDL_Rect* GetRenderRect() {return renderRect;}
    pair<float, float> GetCenter() {return center;}
    bool GetIsPeak() {return isPeak;}
    void SetPeak(bool ip) {isPeak = ip;}
    void SetPosition(float x, float y, bool posOnly = false);
    void SetDefaultPosition(float x, float y);
    virtual void SetCenter(float x = 0, float y = 0, bool centerOnly = false);
    void SetMovable(bool m);
    void SetRendered(bool r);
    void AdjustSize(float multiplier = 1, int w = 0, int h = 0);
    virtual void RenderGameObject(SDL_Renderer* renderer);
    virtual void SetBottomRight();
    pair<float, float> GetBottomRight() {return bottomRight;}
};
class Terrain : public GameObject
{
private:
    int layer;
    float scale;
    double rotation;
    std::vector<Terrain*> neighboringTerrain;

public:
    void RenderGameObject(SDL_Renderer* renderer);

    Terrain(string name, SDL_Texture *texture, SDL_Surface *surface, bool m, bool r, int l) : GameObject(name, texture, surface, m, r) {
        layer = l;
        scale = 1;

    }
    double GetRotation() {return rotation;}
    void SetRotation(double r) {rotation = r;}
    int GetLayer() {return layer;}
    void SetLayer(int l) {layer = l;}
    void SetScale(float s) {scale = s;}
    float GetScale() {return scale;}
    void SetCenter(float x = 0, float y = 0, bool centerOnly = false);
    void SetBottomRight();

};
class GamePiece : public GameObject
{
private:
    int layer;

};

