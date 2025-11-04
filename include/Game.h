#pragma once
#include <SDL3/SDL.h>

#include "Player.h"
#include "Object.h"
#include "FPSTimer.h"
#include "Text.h"
#include "Physics.h"
#include "string"

const float PIXELS_PER_METER = 100.0f;
const float GRAVITY = 9.8f * PIXELS_PER_METER;
const float SIZE = 100.0f;
const int JUMP_COUNT = 2;
const float MOVEMENT_SPEED = 10.0f;
const float WALL_GRAB_FORCE = -0.4f;

class Game
{
public:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    Game();
    ~Game();

    void init();

    Timer<60> timer;

public:
    // Core game loop functions
    void handleInput();
    void update();
    void render();

private:
    // Clear and present
    void _clear(SDL_Color color = {0, 0, 0, 255});
    void _present();

    // Event
    SDL_Event _e;

    // Game Objects
    Player _player;

    Object _test;
    SDL_FRect _testRect;

    Physics2D applyPhysics;
    SDL_FRect screenBox;

    // Screen Dimensions
    int _screenWidth;
    int _screenHeight;

public:
    // Quit flag
    bool running = true;

    Uint64 NOW, LAST;
    double deltaTime;
    float mouseX, mouseY;

    // --- FPS system ---
private:
    double _fpsTimer = 0.0;        // time accumulator for measuring FPS
    int _fpsCounter = 0;           // counts frames in one second
    int _currentFPS = 0;           // calculated FPS
    double _fpsDisplayTimer = 0.0; // timer for updating displayed FPS
    int _displayedFPS = 0;         // stable FPS value shown on screen

    void _updateFPS();
    void _showFPS();
};