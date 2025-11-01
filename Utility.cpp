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
        p.x = sb.w - p.w / 2.0f;
        c.collideRight = true;
    }
    if (p.left < sb.x)
    {
        p.x = p.w / 2.0f;
        c.collideLeft = true;
    }
    if (p.top < sb.y)
    {
        p.y = p.h;
        c.collideTop = true;
    }
    if (p.bottom > sb.h)
    {
        p.y = sb.h - p.h;
        c.collideBottom = true;
    }
}

float clampf(float value, float minVal, float maxVal)
{
    return (value < minVal) ? minVal : (value > maxVal ? maxVal : value);
}

void drawRotatedRect(SDL_Renderer *r, float cx, float cy, float w, float h, float angleDeg)
    {
        float angle = angleDeg * (M_PI / 180.0f);
        float cosA = cos(angle);
        float sinA = sin(angle);

        SDL_Vertex v[4];
        v[0].position = {cx + (-w / 2 * cosA - -h / 2 * sinA), cy + (-w / 2 * sinA + -h / 2 * cosA)};
        v[1].position = {cx + (w / 2 * cosA - -h / 2 * sinA), cy + (w / 2 * sinA + -h / 2 * cosA)};
        v[2].position = {cx + (w / 2 * cosA - h / 2 * sinA), cy + (w / 2 * sinA + h / 2 * cosA)};
        v[3].position = {cx + (-w / 2 * cosA - h / 2 * sinA), cy + (-w / 2 * sinA + h / 2 * cosA)};

        for (int i = 0; i < 4; ++i)
            v[i].color = {255, 255, 255, 255};

        int indices[] = {0, 1, 2, 0, 2, 3};
        SDL_RenderGeometry(r, nullptr, v, 4, indices, 6);
    }