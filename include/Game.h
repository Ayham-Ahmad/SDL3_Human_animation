#pragma once
#include <SDL3/SDL.h>

#include "Player.h"
#include "Object.h"
#include "FPSTimer.h"
#include "Text.h"
#include "Physics.h"
#include <string>
#include "Globals.h"

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

    // Test
    Object _test;
    SDL_FRect _testRect;

    Physics2D _applyPhysics;
    SDL_FRect _screenBox;

    // Screen Dimensions
    int _screenWidth;
    int _screenHeight;

public:
    // Quit flag
    bool running = true;

    Uint64 NOW, LAST;
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