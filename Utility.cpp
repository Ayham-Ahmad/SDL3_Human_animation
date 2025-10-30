#include "Utility.h"
#include <cmath>

bool isCaught(float mx, float my, SDL_FRect box)
{
    return (mx >= box.x && mx <= box.x + box.w &&
            my >= box.y && my <= box.y + box.h);
}

void edgeCollision(Player &p, const SDL_FRect &sb, Collide &c)
{
    if (p.right > sb.w)
    {
        p.X = sb.w - p.W / 2.0f;
        c.collideRight = true;
    }
    if (p.left < sb.x)
    {
        p.X = p.W / 2.0f;
        c.collideLeft = true;
    }
    if (p.top < sb.y)
    {
        p.Y = p.H;
        c.collideTop = true;
    }
    if (p.bottom > sb.h)
    {
        p.Y = sb.h - p.H;
        c.collideBottom = true;
    }
}

float clampf(float value, float minVal, float maxVal)
{
    return (value < minVal) ? minVal : (value > maxVal ? maxVal : value);
}
