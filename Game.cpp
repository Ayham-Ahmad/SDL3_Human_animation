#include "Game.h"

Game::Game()
    : window(nullptr), renderer(nullptr),
      player(0, 0, GRAVITY, SIZE),
      running(true), caught(false), falling(true),
      checkThrowDirection(false), isWallSliding(false),
      isLaunched(false), mouseX(0), mouseY(0),
      mouseX_after(0), lastMouseX(0), lastMouseY(0),
      vx(0.0f), vy(0.0f), jumps(JUMP_COUNT),
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

            if (sc == SDL_SCANCODE_ESCAPE)
                running = false;
            else if (sc == SDL_SCANCODE_SPACE && jumps != 0)
            {
                if (jumps <= JUMP_COUNT)
                {
                    vy = -12.0f;
                    falling = true;
                    jumps -= 1;
                }
            }
        }

        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            if (caught)
                falling = false;

            if (!isLaunched)
            {
                _time = 0;
                isLaunched = true;

                float angle = physics.getVectorDirectionAngle(mouseX, mouseY, player.rightArmXPosition, player.rightArmYPosition);

                vel.x = cos(angle * M_PI / 180);
                vel.y = sin(angle * M_PI / 180) * 100;

                pos.x = player.rightArmXPosition;
                pos.y = player.rightArmYPosition;
            }
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
        {
            if (caught)
            {
                caught = false;
                checkThrowDirection = true;
                releaseTime = SDL_GetTicks();
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
        }
    }

    const bool *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_A] && player.left != screenBox.x)
        player.x -= moveSpeed;
    if (state[SDL_SCANCODE_D] && player.right != screenBox.w)
        player.x += moveSpeed;
}

// --------------------------- UPDATE ---------------------------
void Game::update()
{
    player.updateEdges();
    edgeCollision(hitbox, screenBox, playerCollide);

    if (isLaunched)
        edgeCollision(rock, screenBox, rockCollide);

    handleWallSlide(player, SDL_GetKeyboardState(NULL), vy, isWallSliding, jumps, wallSlideStartTime, screenBox.x, screenBox.w);

    if (caught)
    {
        player.setPosition(mouseX, mouseY);
        vx = vy = 0;
    }

    if (checkThrowDirection)
        throwDir(player, releaseTime, mouseX_after, mouseY, lastMouseX, lastMouseY, thrownDirection, vx, vy, checkThrowDirection, falling);

    if (falling)
        fall(player, vy, thrownDirection, playerCollide, falling, vx, hitbox, screenBox, jumps, JUMP_COUNT);

    if (!rockCollide.collideBottom && !rockCollide.collideTop)
    {
        _time += deltaTime * 15.0f;
        float newRockX = physics.getFinalPosition(0, vel.x, pos.x, _time);
        float newRockY = physics.getFinalPosition(9.81, vel.y, pos.y, _time);
        rock = {newRockX, newRockY, 50, 20};
    }
    else
        isLaunched = false;

    for (auto it = shots.begin(); it != shots.end();)
    {
        float dx = it->end.x - it->start.x;
        float dy = it->end.y - it->start.y;
        float slope = dy / dx;

        if (it->end.x > it->start.x)
            it->start.x += BULLET_SPEED;
        else
            it->start.x -= BULLET_SPEED;

        it->start.y += slope * BULLET_SPEED;

        if (it->start.x < screenBox.x || it->start.x > screenBox.w ||
            it->start.y < screenBox.y || it->start.y > screenBox.h)
        {
            it = shots.erase(it);
            std::cout << "Removed bullet\n";
        }
        else
        {
            ++it;
        }
    }
}

// --------------------------- RENDER ---------------------------
void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    player.updateEdges();
    hitbox = {player.left, player.top, player.w, player.h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderRect(renderer, &hitbox);

    player.stand(mouseX, mouseY);

    if (!isLaunched)
    {
        rock = {player.rightArmXPosition, player.rightArmYPosition, 50, 20};
    }

    SDL_SetRenderDrawColor(renderer, 33, 22, 111, 255);

    SDL_SetRenderDrawColor(renderer, 132, 255, 153, 255);
    for (auto &s : shots)
    {
        SDL_RenderPoint(renderer, s.start.x, s.start.y);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    player.render(renderer);

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
