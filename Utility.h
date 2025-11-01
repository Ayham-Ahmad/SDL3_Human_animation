#ifndef UTILITY_H
#define UTILITY_H

#include <SDL3/SDL.h>
#include "player.h"

bool isCaught(float mx, float my, SDL_FRect box);
void edgeCollision(Player &p, const SDL_FRect &sb, Collide &c);
float clampf(float value, float minVal, float maxVal);
void drawRotatedRect(SDL_Renderer *r, float cx, float cy, float w, float h, float angleDeg);

#endif // UTILITY_H
