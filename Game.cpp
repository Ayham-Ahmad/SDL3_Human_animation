#include "Game.h"

Game::Game()
    : window(nullptr), renderer(nullptr),
      player(0, 0),
      running(true), falling(true),
      checkThrowDirection(false), isWallSliding(false),
      isLaunched(false), mouseX(0), mouseY(0),
      mouseX_after(0), lastMouseX(0), lastMouseY(0),
      jumps(JUMP_COUNT),
      releaseTime(0), wallSlideStartTime(0),
      NOW(SDL_GetPerformanceCounter()), LAST(0),
      deltaTime(0), _time(0)
{
}

// --------------------------- HANDLE INPUT ---------------------------
void Game::handleInput()
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            running = false;

        else if (event.type == SDL_EVENT_KEY_DOWN)
        {
            SDL_Scancode sc = event.key.scancode;

            // Close window
            if (sc == SDL_SCANCODE_ESCAPE)
                running = false;

            // Jumping
            else if (sc == SDL_SCANCODE_SPACE && jumps != 0)
            {
                if (jumps <= JUMP_COUNT)
                {
                    player.playerStatus.vy = -12.0f;
                    falling = true;
                    jumps -= 1;
                }
            }
        }

        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            if (!isLaunched)
            {
                isLaunched = true;

                float angle = applyPhysics.getVectorDirectionAngle(mouseX, mouseY, player.rightArmXPosition, player.rightArmYPosition);

                rockStatus.vx0 = cos(angle * PI / 180);
                rockStatus.vy0 = sin(angle * PI / 180) * 100;

                rockStatus.x0 = player.rightArmXPosition;
                rockStatus.y0 = player.rightArmYPosition;
            }
        }
    }

    const bool *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_A] && !player.playerStatus.collide.collideLeft)
        player.playerStatus.x0 -= MOVEMENT_SPEED;
    if (state[SDL_SCANCODE_D] && !player.playerStatus.collide.collideRight)
        player.playerStatus.x0 += MOVEMENT_SPEED;
}

// --------------------------- UPDATE ---------------------------
void Game::update()
{

    // print(player.x, player.y, playerRect.x, playerRect.y, player.totalTime);

    if (!player.playerStatus.collide.collideBottom)
        player.playerStatus.totalTime += deltaTime;

    player.playerStatus.x = applyPhysics.getFinalPosition(0, player.playerStatus.vx0, player.playerStatus.x0, player.playerStatus.totalTime);
    player.playerStatus.y = applyPhysics.getFinalPosition(GRAVITY, player.playerStatus.vy, player.playerStatus.y0, player.playerStatus.totalTime);

    player.playerStatus.collide.edgeCollision(player.playerStatus, screenBox);

    print(player.playerStatus.x, player.playerStatus.y, player.playerStatus.x0, player.playerStatus.y0, player.playerStatus.totalTime);
    print(player.playerStatus.vx, player.playerStatus.vy, player.playerStatus.vx0, player.playerStatus.vy0);

    player.update();

    // playerRect = {player.x, player.y, player.width, player.height};

    // edgeCollision(playerHitBox, screenBox, playerCollide);

    if (isLaunched)
      rockStatus.collide.edgeCollision(rockStatus, screenBox);

    handleWallSlide(player, SDL_GetKeyboardState(NULL), player.playerStatus.vy, isWallSliding, jumps, wallSlideStartTime, screenBox.x, screenBox.w);

    // if (checkThrowDirection)
    // throwDir(player, releaseTime, mouseX_after, mouseY, lastMouseX, lastMouseY, thrownDirection, vx, vy, checkThrowDirection, falling);

    // if (falling)
    //     fall(player, vy, thrownDirection, playerCollide, falling, vx, playerHitBox, screenBox, jumps, JUMP_COUNT);

    if (!rockStatus.collide.collide)
    {
        _time += deltaTime * 15.0f;
        rockStatus.x = applyPhysics.getFinalPosition(0, rockStatus.vx0, rockStatus.x0, _time);
        rockStatus.y = applyPhysics.getFinalPosition(9.81, rockStatus.vy0, rockStatus.y0, _time);
        rock = {rockStatus.x, rockStatus.y, 50, 20};
    }
    else
        isLaunched = false;

    // for (auto it = shots.begin(); it != shots.end();)
    // {
    //     float dx = it->end.x - it->start.x;
    //     float dy = it->end.y - it->start.y;
    //     float slope = dy / dx;

    //     if (it->end.x > it->start.x)
    //         it->start.x += BULLET_SPEED;
    //     else
    //         it->start.x -= BULLET_SPEED;

    //     it->start.y += slope * BULLET_SPEED;

    //     if (it->start.x < screenBox.x || it->start.x > screenBox.w ||
    //         it->start.y < screenBox.y || it->start.y > screenBox.h)
    //     {
    //         it = shots.erase(it);
    //         std::cout << "Removed bullet\n";
    //     }
    //     else
    //     {
    //         ++it;
    //     }
    // }
}

// --------------------------- RENDER ---------------------------
void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    playerHitBox = {player.left, player.top, player.playerStatus.width, player.playerStatus.height};

    SDL_SetRenderDrawColor(renderer, 0, 100, 22, 255);
    SDL_RenderRect(renderer, &playerHitBox);

    player.stand(mouseX, mouseY);

    if (!isLaunched)
    {
        rock = {rockStatus.x0, rockStatus.y0, 50, 20};
    }

    // SDL_SetRenderDrawColor(renderer, 33, 22, 111, 255);

    // SDL_SetRenderDrawColor(renderer, 132, 255, 153, 255);
    // for (auto &s : shots)
    // {
    //     SDL_RenderPoint(renderer, s.start.x, s.start.y);
    // }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    player.renderPlayerParts(renderer);

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderFillRect(renderer, &rock);

    SDL_RenderPresent(renderer);
}

// --------------------------- OTHERS ---------------------------

float Game::getFramesPerSecond(double deltaTime)
{
    _fpsCounter++;
    _fpsTimer += deltaTime;

    if (_fpsTimer >= 1.0) // every 1 second
    {
        _currentFPS = _fpsCounter / _fpsTimer; // calculate FPS
        _fpsCounter = 0;
        _fpsTimer = 0;
    }

    return _currentFPS;
}