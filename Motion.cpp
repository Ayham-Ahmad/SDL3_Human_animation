#include "Motion.h"
#include "Utility.h"
#include <SDL3/SDL.h>
#include <cmath>

static const float MAX_VERTICAL_FORCE = 20.0f;
static const float MAX_HORIZONTAL_FORCE = 20.0f;

void fall(Player &player, float &vy, ThrownDir &dir,
          Collide &col, bool &falling, float &vx, const SDL_FRect &box,
          const SDL_FRect &sb, int &jumps, const int &JUMP_COUNT)
{
    vy += player.gravity;
    float remaining = sb.h - (box.y + box.h);

    if ((dir.right || dir.left) && !(col.collideLeft || col.collideRight))
        player.X += vx;
    else if (col.collideLeft || col.collideRight)
        vx = 0;

    if (col.collideTop)
        vy = fabs(vy);
    else if (remaining < vy)
    {
        vy = remaining;
        col.collideBottom = true;
    }

    player.Y += vy;

    if (col.collideBottom)
    {
        falling = false;
        jumps = JUMP_COUNT;
        vy = vx = 0;
        dir = ThrownDir();
    }
}

void throwDir(Player &player, Uint32 &releaseTime, float &mouseX_after, float &mouseY, float lastX,
              float lastY, ThrownDir &dir, float &vx, float &vy,
              bool &checkThrow, bool &falling)
{
    if (SDL_GetTicks() - releaseTime < 10)
        return;

    SDL_GetMouseState(&mouseX_after, &mouseY);
    float deltaX = mouseX_after - lastX;
    float deltaY = mouseY - lastY;

    dir.right = deltaX > 0;
    dir.left = deltaX < 0;

    vx = clampf(deltaX * player.gravity, -MAX_HORIZONTAL_FORCE, MAX_HORIZONTAL_FORCE);
    vy = clampf(deltaY * player.gravity, -MAX_VERTICAL_FORCE, MAX_VERTICAL_FORCE);

    checkThrow = false;
    falling = true;
}
