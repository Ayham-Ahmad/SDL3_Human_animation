#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <cmath>

#include "Physics.h"
#include "Object.h"

class Player
{
public:
    Player(float screenW, float screenH);

    void update();
    void stand(float mx, float my);
    void updateArmAngles(float mx, float my);
    void renderPlayerParts(SDL_Renderer *r);

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
    Object playerStatus;

    float top, bottom, left, right;

// private:
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
