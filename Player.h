#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <cmath>

#include "Physics.h"

using namespace std;

#define M_PI 3.14159265358979323846

class Collide
{
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;

    void reset();
};

class ThrownDirection
{
public:
    bool right = false;
    bool left = false;
};

class Player
{
public:
    Player(float screenW, float screenH, float gravityValue, float size);

    void setPosition(float x, float y);
    void updateEdges();
    void stand(float mx, float my);
    void updateArmAngles(float mx, float my);
    void render(SDL_Renderer *r);

private:
    // internal drawing helpers
    void renderHead(SDL_Renderer *r);
    void renderNeck(SDL_Renderer *r);
    void renderBody(SDL_Renderer *r);
    void renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide);

private:
    float screenWidth;
    float screenHeight;

    Physics2D _apply2DPhysics;

public:
    float x, y;    // center position
    float w, h;    // width, height
    float gravity; // gravity value
    float top, bottom, left, right;

    // Body parts
    float headYPosition;
    float neckStartPoint;
    float neckEndPoint;
    float bodyYEndPoint;

    // Arms & legs
    float rightArmXPosition, rightArmYPosition;
    float leftArmXPosition, leftArmYPosition;
    float armLength;
    float legsAngle;
    float legsLength;
};
