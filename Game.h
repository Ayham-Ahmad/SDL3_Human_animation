#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>
#include "FPSTimer.h"
#include "player.h"
#include "Utility.h"
#include "Motion.h"
#include "Weapons.h"
#include "Physics.h"

using namespace std;

#define M_PI 3.14159265358979323846

// Constants
const float moveSpeed = 10.0f;
const float WALL_GRAB_FORCE = -0.4f;
const int JUMP_COUNT = 2;
const float GRAVITY = 0.5f;
const float SIZE = 200.0f;
const int BULLET_SPEED = 15.0f;

// Structs
struct Shot
{
    SDL_FPoint start;
    SDL_FPoint end;
};

struct _initialVelocity
{
    float x;
    float y;
};

struct _initialPosition
{
    float x;
    float y;
};

// Game class
class Game
{

private:
    double _fpsTimer = 0;
    int _fpsCounter = 0;
    float _currentFPS = 0;

public:
    // SDL objects
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_FRect hitbox;
    SDL_FRect screenBox;
    SDL_Event event;

    // Player & physics
    Player player;
    Collide playerCollide;
    Collide rockCollide;
    ThrownDirection thrownDirection;
    Weapon weapon;
    Physics2D physics;

    // Game state
    bool running, caught, falling, checkThrowDirection, isWallSliding;
    bool isLaunched;
    float mouseX, mouseY, mouseX_after;
    float lastMouseX, lastMouseY;
    float vx, vy;
    int jumps;
    Uint32 releaseTime;
    Uint64 wallSlideStartTime;
    Uint64 NOW, LAST;
    double deltaTime;
    double _time;

    vector<Shot> shots;
    SDL_FRect rock;
    _initialVelocity vel;
    _initialPosition pos;

    Timer<60> timer; // 60 FPS limiter

    // Constructor
    Game();

    // Main loop functions
    void handleInput();
    void update();
    void render();

    float getFramesPerSecond(double deltaTime);
};

#endif // GAME_H
