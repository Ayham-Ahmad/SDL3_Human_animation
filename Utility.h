#ifndef UTILITY_H
#define UTILITY_H

#include <SDL3/SDL.h>
#include "Player.h"

bool isCaught(float mx, float my, SDL_FRect box);
void edgeCollision(Player &p, const SDL_FRect &sb, Collide &c);
float clampf(float value, float minVal, float maxVal);

#endif // UTILITY_H
