#pragma once
#include <SDL3/SDL.h>

struct Object;

class Collide
{
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;
    bool collide = false;

    void reset()
    {
        collideBottom = collideTop = collideRight = collideLeft = collide = false;
    }

    void edgeCollision(Object &obj, const SDL_FRect &bounds);
};

struct Object
{
    float x0 = 0.0f;  // initial x position
    float y0 = 0.0f;  // initial y position
    float vx0 = 0.0f; // initial horizontal velocity
    float vy0 = 0.0f; // initial vertical velocity

    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float ax = 0.0f;
    float ay = 0.0f;
    float width = 50.0f;
    float height = 100.0f;
    float mass = 1.0f;

    double totalTime = 0.0;

    bool affectedByGravity = true;

    Collide collide;
};

inline void Collide::edgeCollision(Object &obj, const SDL_FRect &bounds)
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
