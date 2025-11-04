#pragma once
#include <SDL3/SDL.h>

#include "Utility.h"
#include "Physics.h"
#include "Object.h"

class Player
{
public:
    Object _playerPhysics;
    Physics2D _applyPhysics;

public:
    Player(const float screenW, const float screenH, const int SIZE);
    Player() {}

    void update(const float mx, const float my, const double deltaTime, const double GRAVITY, const int JUMP_COUNT, SDL_FRect screenBox);
    void stand(float mx, float my);
    void renderPlayerParts(SDL_Renderer *r);

private:
    void _updateArmAngles(float mx, float my);

    // internal drawing helpers
    void _renderHead(SDL_Renderer *r);
    void _renderNeck(SDL_Renderer *r);
    void _renderBody(SDL_Renderer *r);
    void _renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide);

    float _screenWidth;
    float _screenHeight;

    float mouseX;
    float mouseY;

    int _SIZE;

public:
    float top, bottom, left, right;

    int jumps;

    SDL_FRect playerHitBox;
    Object playerHitBoxObj;

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