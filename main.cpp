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
#include "Physics.h"

using namespace std;

#define M_PI 3.14159265358979323846

const float moveSpeed = 10.0f;
const float WALL_GRAB_FORCE = -0.4f;
const int JUMP_COUNT = 2;
const float GRAVITY = 0.5f;
const float SIZE = 200.0f;
const int BULLET_SPEED = 15.0f;

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

    bool running = true, caught = false, falling = true, checkThrowDirection = false, isWallSliding = false;
    float mouseX = 0, mouseY = 0, mouseX_after = 0;
    float lastMouseX = player.x, lastMouseY = player.y;
    int jumps = JUMP_COUNT;

    bool isLaunched = false;

    float vx = 0.0f, vy = 0.0f;

    SDL_Event event;
    SDL_FRect screenBox = {0, 0, (float)screenWidth, (float)screenHeight};

    Collide playerCollide;
    Collide rockCollide;
    ThrownDirection thrownDirection;
    Weapon weapon;
    Physics2D physics;

    Uint32 releaseTime = 0;
    Uint64 wallSlideStartTime = 0;

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

    _initialVelocity vel;
    _initialPosition pos;

    vector<Shot> shots;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    double _time = 0; // <-- total elapsed time you can use later

    SDL_FRect rock;

    Timer<60> timer; // 60 FPS limiter

    while (running)
    {
        SDL_GetMouseState(&mouseX, &mouseY);

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency()); // milliseconds

        float angle = physics.getVectorDirectionAngle(mouseX, mouseY, player.rightArmXPosition, player.rightArmYPosition);
        // float magnitude = physics.getVectorMagnitude(mouseX, mouseY, player.x, player.y);

        // float x = physics.getXForVectorUsingAngleAndMagnitude(angle, magnitude);
        // float y = physics.getYForVectorUsingAngleAndMagnitude(angle, magnitude);

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

                if (!isLaunched)
                {
                    _time = 0;
                    isLaunched = true;

                    vel.x = cos(angle * M_PI / 180);
                    vel.y = sin(angle * M_PI / 180) * 100;

                    pos.x = player.rightArmXPosition;
                    pos.y = player.rightArmYPosition;
                }

                // if (shots.size() < weapon.getAmmoCount())
                // {
                //     Shot s;
                //     s.start.x = player.x;
                //     s.start.y = player.neckEndPoint;
                //     s.end.x = mouseX;
                //     s.end.y = mouseY;
                //     shots.push_back(s);

                //     cout << "new shot" << endl;
                // }
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

        // update edges before collision check
        player.updateEdges();

        edgeCollision(hitbox, screenBox, playerCollide);
        if (isLaunched)
            edgeCollision(rock, screenBox, rockCollide);

        handleWallSlide(player, state, vy, isWallSliding, jumps, wallSlideStartTime, screenBox.x, screenBox.w);

        if (caught)
        {
            player.setPosition(mouseX, mouseY);
            vx = vy = 0;
        }

        if (checkThrowDirection)
            throwDir(player, releaseTime, mouseX_after, mouseY, lastMouseX, lastMouseY, thrownDirection, vx, vy, checkThrowDirection, falling);

        if (falling)
            fall(player, vy, thrownDirection, playerCollide, falling, vx, hitbox, screenBox, jumps, JUMP_COUNT);

        // --- Rendering ---
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
        // SDL_RenderLine(renderer, player.x, player.y, mouseX, mouseY);

        SDL_SetRenderDrawColor(renderer, 132, 255, 153, 255);

        // for (auto it = shots.begin(); it != shots.end();)
        // {
        //     float dx = it->end.x - it->start.x;
        //     float dy = it->end.y - it->start.y;

        //     // Get slope direction (like tan)
        //     float slope = dy / dx;

        //     // Move in X direction
        //     if (it->end.x > it->start.x)
        //         it->start.x += BULLET_SPEED;
        //     else
        //         it->start.x -= BULLET_SPEED;

        //     // Move Y based on slope
        //     it->start.y += slope * BULLET_SPEED;

        //     SDL_RenderPoint(renderer, it->start.x, it->start.y);

        //     // Remove if bullet is outside the screen
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

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        player.render(renderer);

        if (!rockCollide.collideBottom && !rockCollide.collideTop)
        {
            _time += deltaTime * 15.0f;

            float newRockX = physics.getFinalPosition(0, vel.x, pos.x, _time);
            float newRockY = physics.getFinalPosition(-9.81, vel.y, pos.y, _time);

            rock = {newRockX, newRockY, 50, 20};

            cout << newRockX << endl;
            cout << newRockY << endl;
        }
        else
            isLaunched = false;

        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderFillRect(renderer, &rock);

        // weapon.pistol(renderer, player);

        SDL_RenderPresent(renderer);
        timer.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


// still not working for throwing
// try to make new make to detected collision