#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <iostream>

static std::unordered_map<int, std::vector<SDL_FPoint>> circleCache;

inline void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
{
    int r = static_cast<int>(radius);
    if (r <= 0)
        return;

    if (circleCache.find(r) == circleCache.end())
    {
        std::vector<SDL_FPoint> points;
        int x = r, y = 0, decision = 1 - x;

        while (y <= x)
        {
            points.push_back({(float)x, (float)y});
            points.push_back({(float)y, (float)x});
            points.push_back({(float)-y, (float)x});
            points.push_back({(float)-x, (float)y});
            points.push_back({(float)-x, (float)-y});
            points.push_back({(float)-y, (float)-x});
            points.push_back({(float)y, (float)-x});
            points.push_back({(float)x, (float)-y});

            y++;
            if (decision <= 0)
                decision += 2 * y + 1;
            else
                decision += 2 * (y - --x) + 1;
        }

        circleCache[r] = std::move(points);
    }

    const auto &points = circleCache[r];
    for (const auto &p : points)
        SDL_RenderPoint(renderer, cx + p.x, cy + p.y);
}

// Printing
template <typename T>
void print(const T &value)
{
    std::cout << value << std::endl;
}

template <typename T, typename... Args>
void print(const T &first, const Args &...rest)
{
    std::cout << first << " ";
    print(rest...);
}