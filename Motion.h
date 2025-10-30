#ifndef MOTION_H
#define MOTION_H

#include <SDL3/SDL.h>
#include "Player.h"

// motion functions now use player.gravity passed from the Player instance
void fall(Player &player, float &vy, ThrownDir &dir,
          Collide &col, bool &falling, float &vx, const SDL_FRect &box,
          const SDL_FRect &sb, int &jumps, const int &JUMP_COUNT);

void throwDir(Player &player, Uint32 &releaseTime, float &mouseX_after, float &mouseY, float lastX,
              float lastY, ThrownDir &dir, float &vx, float &vy,
              bool &checkThrow, bool &falling);

#endif // MOTION_H
