#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "FPSTimer.h"
#include "Player.h"
#include "Utility.h"
#include "Motion.h"

using namespace std;

const float moveSpeed = 10.0f;
const float WALL_GRAB_FORCE = -0.4f;
const int JUMP_COUNT = 2;
const float GRAVITY = 0.5f;
const float SIZE  = 200.0f;

int SDL_main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Human", 0, 0, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
    const int screenWidth = mode->w;
    const int screenHeight = mode->h;

    Player player(screenWidth, screenHeight, GRAVITY, SIZE);
    SDL_FRect hitbox = {player.left, player.top, player.W, player.H};

    bool running = true, caught = false, falling = true, checkThrowDir = false, isWallSliding = false;
    float mouseX = 0, mouseY = 0, mouseX_after = 0;
    float lastMouseX = player.X, lastMouseY = player.Y;
    int jumps = JUMP_COUNT;

    float HY;      // Head Y position
    float NSP;     // Neck start
    float NEP;     // Neck end
    float HLL;     // limb length
    float RHAngle; // right hand angle
    float LHAngle; // left hand angle
    float RLAngle; // right leg angle
    float LLAngle; // left leg angle
    float BEP;     // body end point

    stand(player, HY, NSP, NEP, HLL, RHAngle, LHAngle, RLAngle, LLAngle, BEP);

    float vx = 0.0f, vy = 0.0f;

    SDL_Event event;
    SDL_FRect screenBox = {0, 0, (float)screenWidth, (float)screenHeight};

    Collide collideChecker;
    ThrownDir thrownDir;

    Uint32 releaseTime = 0;
    Uint64 wallSlideStartTime = 0;

    Timer<60> timer; // 60 FPS limiter

    while (running)
    {
        SDL_GetMouseState(&mouseX, &mouseY);

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
                caught = isCaught(mouseX, mouseY, hitbox);
                if (caught)
                    falling = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
            {
                if (caught)
                {
                    caught = false;
                    checkThrowDir = true;
                    releaseTime = SDL_GetTicks();
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
            }
        }

        const bool *state = SDL_GetKeyboardState(NULL);

        if (state[SDL_SCANCODE_A] && player.left != screenBox.x)
            player.X -= moveSpeed;
        if (state[SDL_SCANCODE_D] && player.right != screenBox.w)
            player.X += moveSpeed;

        // update edges before collision check
        player.updateEdges();

        edgeCollision(player, screenBox, collideChecker);

        // wall slide detection (touching left or right wall AND holding toward it)
        if ((state[SDL_SCANCODE_A] && player.left == screenBox.x) ||
            (state[SDL_SCANCODE_D] && player.right == screenBox.w))
        {
            isWallSliding = true;
            if (vy > 2.0f)
                vy = 2.0f;
        }
        else
        {
            isWallSliding = false;
        }

        // regain jump after sliding on wall for 1.5s
        if (isWallSliding && jumps == 0)
        {
            if (wallSlideStartTime == 0)
                wallSlideStartTime = SDL_GetTicks();

            if (SDL_GetTicks() - wallSlideStartTime >= 1500)
            {
                jumps += 1;
                wallSlideStartTime = 0;
            }
        }
        else
        {
            wallSlideStartTime = 0;
        }

        if (caught)
        {
            player.setPosition(mouseX, mouseY);
            vx = vy = 0;
        }

        if (checkThrowDir)
            throwDir(player, releaseTime, mouseX_after, mouseY, lastMouseX, lastMouseY, thrownDir, vx, vy, checkThrowDir, falling);

        if (falling)
            fall(player, vy, thrownDir, collideChecker, falling, vx, hitbox, screenBox, jumps, JUMP_COUNT);

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        collideChecker.reset();

        player.updateEdges();

        hitbox = {player.left, player.top, player.W, player.H};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderRect(renderer, &hitbox);

        edgeCollision(player, screenBox, collideChecker);

        stand(player, HY, NSP, NEP, HLL, RHAngle, LHAngle, RLAngle, LLAngle, BEP);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        getHuman(renderer, player, HY, NSP, NEP, HLL, RHAngle, LHAngle, BEP, RLAngle, LLAngle);

        SDL_RenderPresent(renderer);
        timer.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
