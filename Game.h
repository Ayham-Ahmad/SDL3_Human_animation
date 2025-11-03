#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>

struct Object;
class Collide;

#include "FPSTimer.h"
#include "Player.h"
#include "Utility.h"
#include "Motion.h"
#include "Weapons.h"
#include "Physics.h"

using namespace std;

// Constants
const float PIXELS_PER_METER = 100.0f;
const float GRAVITY = 9.8f * PIXELS_PER_METER;
const float MOVEMENT_SPEED = 10.0f;
const float WALL_GRAB_FORCE = -0.4f;
const int JUMP_COUNT = 2;
const float SIZE = 100.0f;
const int BULLET_SPEED = 15.0f;

// Game class
class Game
{

private:
    // To calculate frames
    double _fpsTimer = 0;
    int _fpsCounter = 0;
    float _currentFPS = 0;

public:
    // SDL objects
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_FRect playerHitBox;
    SDL_FRect screenBox;
    SDL_Event event;

    // Player & physics
    Player player;
    // ThrownDirection thrownDirection;
    Weapon weapon;
    Physics2D applyPhysics;
    Object rockStatus;

    // Game state
    bool running, falling, checkThrowDirection, isWallSliding;
    bool isLaunched;
    float mouseX, mouseY, mouseX_after;
    float lastMouseX, lastMouseY;
    int jumps;
    Uint32 releaseTime;
    Uint64 wallSlideStartTime;
    Uint64 NOW, LAST;
    double deltaTime;
    double _time;

    // vector<Shot> shots;
    SDL_FRect rock;

    Timer<60> timer; // 60 FPS limiter

    // Constructor
    Game();

    // Main loop functions
    void handleInput();
    void update();
    void render();

    float getFramesPerSecond(double deltaTime);
};

// Printing
template <typename T>
void print(const T &value)
{
    cout << value << endl;
}

template <typename T, typename... Args>
void print(const T &first, const Args &...rest)
{
    cout << first << " ";
    print(rest...);
}

#endif // GAME_H
