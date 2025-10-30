#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <cmath>

// ============================================================
// Circle.h — Efficient, cached circle renderer using SDL3
// ============================================================

class Circle
{
private:
    std::vector<SDL_Vertex> vertices;
    bool initialized = false;

public:
    Circle() = default;

    bool isBuilt() const { return initialized; }

    // Build circle geometry once (radius in pixels). color uses SDL_FColor.
    void build(float radius, const SDL_FColor &fcolor, int segments = 64)
    {
        if (initialized) return;
        if (segments < 8) segments = 8;

        vertices.clear();
        vertices.reserve(segments + 2);

        // center vertex (triangle fan)
        SDL_Vertex center{};
        center.position.x = 0.0f;
        center.position.y = 0.0f;
        center.color = fcolor;
        center.tex_coord.x = 0.0f;
        center.tex_coord.y = 0.0f;
        vertices.push_back(center);

        constexpr float PI = 3.14159265358979323846f;
        for (int i = 0; i <= segments; ++i)
        {
            float theta = (float)i / segments * 2.0f * PI;
            SDL_Vertex v{};
            v.position.x = cosf(theta) * radius;
            v.position.y = sinf(theta) * radius;
            v.color = fcolor;
            v.tex_coord.x = 0.0f;
            v.tex_coord.y = 0.0f;
            vertices.push_back(v);
        }

        initialized = true;
    }

    // Draw the prebuilt circle at (cx, cy)
    void draw(SDL_Renderer *renderer, float cx, float cy) const
    {
        if (!initialized || vertices.empty()) return;

        // Copy & translate vertices (we must not modify original cache)
        std::vector<SDL_Vertex> transformed;
        transformed.reserve(vertices.size());
        for (const auto &v : vertices)
        {
            SDL_Vertex tv = v;
            tv.position.x += cx;
            tv.position.y += cy;
            transformed.push_back(tv);
        }

        SDL_RenderGeometry(renderer, nullptr,
                           transformed.data(), static_cast<int>(transformed.size()),
                           nullptr, 0);
    }
};
