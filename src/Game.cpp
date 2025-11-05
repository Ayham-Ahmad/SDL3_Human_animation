#include "Game.h"

// Constructor / Destructor
Game::Game()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
}

Game::~Game()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void Game::init()
{
    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);

    _screenWidth = mode->w;
    _screenHeight = mode->h;

    _player = Player(_screenWidth, _screenHeight);

    // Create window and renderer
    window = SDL_CreateWindow("Human", _screenWidth, _screenHeight, SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window, nullptr);

    _screenBox = {0, 0, (float)_screenWidth, (float)_screenHeight};

    // Center test rectangle
    _test.x0 = _screenWidth / 3.0f;
    _test.y0 = _screenHeight / 2.0f;
    _test.width = 10;
    _test.height = 10;
}

// --- FPS Handling ---
void Game::_updateFPS()
{
    _fpsCounter++;
    _fpsTimer += deltaTime;
    _fpsDisplayTimer += deltaTime;

    // Calculate FPS once per second
    if (_fpsTimer >= 1.0)
    {
        _currentFPS = static_cast<int>(_fpsCounter / _fpsTimer);
        _fpsCounter = 0;
        _fpsTimer = 0.0;
    }

    // Update displayed FPS once per second (for smooth text)
    if (_fpsDisplayTimer >= 1.0)
    {
        _displayedFPS = _currentFPS;
        _fpsDisplayTimer = 0.0;
    }
}

void Game::_showFPS()
{
    drawText(renderer, "FPS: " + std::to_string(_displayedFPS), 10, 10, 25);
}

// --- clear and present ---
void Game::_clear(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void Game::_present()
{
    SDL_RenderPresent(renderer);
}

// --- handleInput: only input handling ---
void Game::handleInput()
{
    while (SDL_PollEvent(&_e))
    {
        // Close window
        if (_e.type == SDL_EVENT_QUIT)
            running = false;
        else if (_e.type == SDL_EVENT_KEY_DOWN)
        {
            SDL_Scancode sc = _e.key.scancode;

            // Close window
            if (sc == SDL_SCANCODE_ESCAPE)
                running = false;

            // Jumping
            else if (sc == SDL_SCANCODE_SPACE)
            {
                _player.jump();
            }
        }
    }

    const bool *state = SDL_GetKeyboardState(NULL); // For continuos pressing

    _player.handleHorizontalMovement(state); // Move the player left and right
}

// --- update: only game state changes ---
void Game::update()
{
    if (!_test.collide.collide)
    {
        _test.totalTime += deltaTime;
        _test.x = _applyPhysics.getFinalPosition(0, _test.vx0, _test.x0, _test.totalTime);
        _test.y = _applyPhysics.getFinalPosition(GRAVITY, _test.vy0, _test.y0, _test.totalTime);
        _test.collide.edgeCollision(_test, _screenBox);
        // print("vx:", _test.vx, " vy:", _test.vy, " x:", _test.x, " y:", _test.y, " totalTime:", _test.totalTime);
    }

    _player.update(mouseX, mouseY, _screenBox);

    if (SDL_HasRectIntersectionFloat(&_player.playerHitBox, &_testRect))
    {
        print("Collision detected!");
        _player.jump();
        }
    else
    {
        printf("No collision.\n");
    }

    _updateFPS();
}

// --- render: only drawing ---
void Game::render()
{
    _clear({0, 0, 0, 255});

    // Test
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    _testRect = {_test.x, _test.y, _test.width, _test.height};
    SDL_RenderFillRect(renderer, &_testRect);
    showXAndYPosition(renderer, _test.x, _test.y);

    // Player
    _player.renderPlayerParts(renderer);
    showXAndYPosition(renderer, _player._Rigidbody.x, _player.top, -20);

    // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    // SDL_RenderRect(renderer, &_player.playerHitBox);

    _showFPS();

    _present();
}
