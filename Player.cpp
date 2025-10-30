#include "Player.h"
#include <unordered_map>
#include <vector>
#include <cmath>

// cache for circle drawing
static std::unordered_map<int, std::vector<SDL_FPoint>> circleCache;

// default size constant used by drawing helpers (kept same as original)
static const float DEFAULT_SIZE_FOR_DRAW = 200.0f;

Player::Player(float screenW, float screenH, float gravityValue, float size)
    : screenWidth(screenW), screenHeight(screenH), H(size), W(size / 2.0f), gravity(gravityValue)
{
    X = screenWidth / 2.0f;
    Y = screenHeight / 2.0f;
    updateEdges();
}

void Player::updateEdges()
{
    top = Y - H / 2;
    bottom = Y + H / 2;
    right = X + W / 2;
    left = X - W / 2;
}

void Player::setPosition(float x, float y)
{
    X = x;
    Y = y;
    updateEdges();
}

void Collide::reset()
{
    collideBottom = collideTop = collideRight = collideLeft = false;
}

// --- drawing helpers (ported exactly from your original code) ---

void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
{
    int r = static_cast<int>(radius);

    if (r <= 0)
        return;

    // build cache if missing
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

inline void head(SDL_Renderer *r, float x, float y)
{
    SDL_RenderCircle(r, x, y, DEFAULT_SIZE_FOR_DRAW / 7.0f);
}

inline void nick(SDL_Renderer *r, float x, float y1, float y2)
{
    SDL_RenderLine(r, x, y1, x, y2);
}

void body(SDL_Renderer *renderer, float x1, float y1, float x2, float y2)
{
    SDL_RenderLine(renderer, x1, y1, x2, y2);
}

inline void limb(SDL_Renderer *r, float x, float y, float angle, float length, bool rightSide)
{
    float x2 = rightSide ? x + angle : x - angle;
    SDL_RenderLine(r, x, y, x2, y + length);
}

void stand(Player &p, float &HY, float &NSP, float &NEP, float &HLL,
           float &RHAngle, float &LHAngle, float &RLAngle, float &LLAngle, float &BEP)
{
    HY = p.Y - p.H * 0.35f;  // Head Y center, slightly above middle
    NSP = HY + p.H * 0.15f;  // Neck start (below head)
    NEP = NSP + p.H * 0.1f;  // Neck end (joins torso)
    BEP = NEP + p.H * 0.35f; // Bottom of torso
    HLL = p.H * 0.2f;        // Limb length proportion (arms & legs)

    RHAngle = LHAngle = 45.0f; // Arms angled outward
    RLAngle = LLAngle = 30.0f; // Legs angled downward
}

void getHuman(SDL_Renderer *r, const Player &p, float HY, float NSP, float NEP, float HLL,
              float RHAngle, float LHAngle, float BEP, float RLAngle, float LLAngle)
{
    head(r, p.X, HY);
    nick(r, p.X, NSP, NEP);
    limb(r, p.X, NEP, RHAngle, HLL, false);
    limb(r, p.X, NEP, LHAngle, HLL, true);
    body(r, p.X, NEP, p.X, BEP);
    limb(r, p.X, BEP, RLAngle, HLL + 8, false);
    limb(r, p.X, BEP, LLAngle, HLL + 8, true);
}
