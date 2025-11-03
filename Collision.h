#ifndef COLLISION_H
#define COLLISION_H

#pragma once
#include <SDL3/SDL.h>
#include "Object.h"

class Collide
{
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;
    bool collide = false;

    void reset() { collideBottom = collideTop = collideRight = collideLeft = collide = false; }

    void edgeCollision(Object &obj, const SDL_FRect &bounds)
    {
        reset();

        // Right edge
        if (obj.x + obj.width > bounds.x + bounds.w)
        {
            obj.x = bounds.x + bounds.w - obj.width;
            collideRight = collide = true;
        }

        // Left edge
        if (obj.x < bounds.x)
        {
            obj.x = bounds.x;
            collideLeft = collide = true;
        }

        // Top edge
        if (obj.y < bounds.y)
        {
            obj.y = bounds.y;
            collideTop = collide = true;
        }

        // Bottom edge
        if (obj.y + obj.height > bounds.y + bounds.h)
        {
            obj.y = bounds.y + bounds.h - obj.height;
            collideBottom = collide = true;
        }
    }
};

#endif // COLLISION_H