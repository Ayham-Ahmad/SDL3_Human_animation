#pragma once
#include <SDL3/SDL.h>
#include <cmath>

#include "Utility.h"
#include "Physics.h"
#include "Object.h"
#include "Globals.h"

class Player
{
public:
    Object _Rigidbody;
    Physics2D _applyPhysics;

public:
    Player(const float screenW, const float screenH);
    Player() {}

    // Control
    void jump();
    void handleHorizontalMovement(const bool *state);

    // uplate
    void update(const float mx, const float my, SDL_FRect screenBox);
    void _updateStandingState();
    void renderPlayerParts(SDL_Renderer *r);

private:
    void _updateArmAngles(float mx, float my);
    void _updateHitboxFromRigidbody();
    void _syncRigidbodyWithHitbox();
    void _handleCollisionResponse();
    void _updateHitboxEdges();
    void _handleWallSlide();

    // Body drawing helpers
    void _renderHead(SDL_Renderer *r);
    void _renderNeck(SDL_Renderer *r);
    void _renderBody(SDL_Renderer *r);
    void _renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide);

    float _screenWidth;
    float _screenHeight;

    float mouseX;
    float mouseY;

    int _jumps;

    bool _inAir = false;

public:
    float top, bottom, left, right;

    SDL_FRect playerHitBox;
    Object playerHitBoxRigidbody;

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