#include "player.h"
#include <unordered_map>
#include <vector>
#include <cmath>

static std::unordered_map<int, std::vector<SDL_FPoint>> circleCache;
static const float DEFAULT_SIZE_FOR_DRAw = 200.0f;

Player::Player(float screenW, float screenH, float gravityValue, float size)
    : screenWidth(screenW), screenHeight(screenH), h(size), w(size / 2.0f), gravity(gravityValue)
{
    x = screenWidth / 2.0f;
    y = screenHeight / 2.0f;
    updateEdges();
}

void Player::updateEdges()
{
    top = y - h / 2;
    bottom = y + h / 2;
    right = x + w / 2;
    left = x - w / 2;
}

void Player::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
    updateEdges();
}

void Collide::reset()
{
    collideBottom = collideTop = collideRight = collideLeft = false;
}

void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
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

inline void head(SDL_Renderer *r, float x, float y)
{
    SDL_RenderCircle(r, x, y, DEFAULT_SIZE_FOR_DRAw / 7.0f);
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

void updateArmAngles(float &mx, float &my, Player &p)
{
    float dx = mx - p.x;
    float dy = my - p.y;
    float angle = atan2(dy, dx);

    cout << angle * 180.0 / M_PI << endl;

    p.RAX = p.x + cos(angle) * p.HL;
    p.RAY = p.NEP + sin(angle) * p.HL;

    float leftArmAngle = angle + M_PI / 8;
    p.LAX = p.x + cos(leftArmAngle) * p.HL;
    p.LAY = p.NEP + sin(leftArmAngle) * p.HL;
}

void stand(Player &p, float &mx, float &my)
{
    p.HY = p.y - p.h * 0.35f;
    p.NSP = p.HY + p.h * 0.15f;
    p.NEP = p.NSP + p.h * 0.1f;
    p.BEP = p.NEP + p.h * 0.35f;
    p.HL = p.h * 0.2f;
    p.LA = 30;
    p.LL = p.HL + 8;

    updateArmAngles(mx, my, p);
}

void getHuman(SDL_Renderer *r, const Player &p)
{
    head(r, p.x, p.HY);
    nick(r, p.x, p.NSP, p.NEP);
    limb(r, p.x, p.NEP, p.RAX - p.x, p.RAY - p.NEP, true);
    limb(r, p.x, p.NEP, p.LAX - p.x, p.LAY - p.NEP, true);
    body(r, p.x, p.NEP, p.x, p.BEP);
    limb(r, p.x, p.BEP, p.LA, p.LL, false);
    limb(r, p.x, p.BEP, p.LA, p.LL, true);
}
