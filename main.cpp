#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "FPSTimer.h"
#include "player.h"
#include "Utility.h"
#include "Motion.h"
#include "Weapons.h"

using namespace std;

#define M_PI 3.14159265358979323846

const float moveSpeed = 10.0f;
const float WALL_GRAB_FORCE = -0.4f;
const int JUMP_COUNT = 2;
const float GRAVITY = 0.5f;
const float SIZE = 200.0f;
const int BULLET_SPEED = 5;

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
    SDL_FRect hitbox = {player.left, player.top, player.w, player.h};

    bool running = true, caught = false, falling = true, checkThrowDir = false, isWallSliding = false;
    float mouseX = 0, mouseY = 0, mouseX_after = 0;
    float lastMouseX = player.x, lastMouseY = player.y;
    int jumps = JUMP_COUNT;

    float vx = 0.0f, vy = 0.0f;

    SDL_Event event;
    SDL_FRect screenBox = {0, 0, (float)screenWidth, (float)screenHeight};

    Collide collideChecker;
    ThrownDir thrownDir;
    Weapon weapon;

    Uint32 releaseTime = 0;
    Uint64 wallSlideStartTime = 0;

    struct Shot
    {
        SDL_FPoint start;
        SDL_FPoint end;
    };

    vector<Shot> shots;

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
                // caught = isCaught(mouseX, mouseY, hitbox);
                if (caught)
                    falling = false;

                Shot s;
                s.start.x = player.x;
                s.start.y = player.NEP;
                s.end.x = mouseX;
                s.end.y = mouseY;
                shots.push_back(s);
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
            player.x -= moveSpeed;
        if (state[SDL_SCANCODE_D] && player.right != screenBox.w)
            player.x += moveSpeed;

        // update edges before collision check
        player.updateEdges();

        edgeCollision(player, screenBox, collideChecker);

        handleWallSlide(player, state, vy, isWallSliding, jumps, wallSlideStartTime, screenBox.x, screenBox.w);

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

        hitbox = {player.left, player.top, player.w, player.h};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &hitbox);

        stand(player, mouseX, mouseY);

        SDL_SetRenderDrawColor(renderer, 33, 22, 111, 255);
        // SDL_RenderLine(renderer, player.x, player.NEP, mouseX, mouseY);

        SDL_SetRenderDrawColor(renderer, 132, 255, 153, 255);
        const float BULLET_SPEED = 15.0f;

        for (auto it = shots.begin(); it != shots.end();)
        {
            float dx = it->end.x - it->start.x;
            float dy = it->end.y - it->start.y;
            float length = std::sqrt(dx * dx + dy * dy);

            // Normalize direction
            float dirX = dx / length;
            float dirY = dy / length;

            // Move bullet forward
            it->start.x += dirX * BULLET_SPEED;
            it->start.y += dirY * BULLET_SPEED;

            SDL_RenderPoint(renderer, it->start.x, it->start.y);

            // Remove if out of screen
            if (it->start.x < screenBox.x || it->start.x > screenBox.w ||
                it->start.y < screenBox.y || it->start.y > screenBox.h)
            {
                it = shots.erase(it);
            }
            else
            {
                ++it;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        getHuman(renderer, player);

        weapon.pistol(renderer, player);

        SDL_RenderPresent(renderer);
        timer.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
