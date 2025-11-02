#include "Player.h"
#include <cmath>
#include <unordered_map>
#include <vector>

static std::unordered_map<int, std::vector<SDL_FPoint>> circleCache;

void Collide::reset()
{
    collideBottom = collideTop = collideRight = collideLeft = false;
}

// ---------------------------
// Generic helper (outside Player)
// ---------------------------
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

// ---------------------------
// Player methods
// ---------------------------

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

void Player::stand(float mx, float my)
{
    headYPosition = y - h * 0.35f;
    neckStartPoint = headYPosition + h * 0.15f;
    neckEndPoint = neckStartPoint + h * 0.1f;
    bodyYEndPoint = neckEndPoint + h * 0.35f;
    armLength = h * 0.2f;
    legsAngle = 30;
    legsLength = armLength + 8;
    updateArmAngles(mx, my);
}

void Player::updateArmAngles(float mx, float my)
{
    // Get base angle toward the mouse
    float angle = _apply2DPhysics.getVectorDirectionAngle(mx, my, x, neckEndPoint);

    // Right arm directly points toward the mouse
    rightArmXPosition = _apply2DPhysics.getXForVectorUsingAngleAndMagnitude(angle, armLength, x);
    rightArmYPosition = _apply2DPhysics.getYForVectorUsingAngleAndMagnitude(angle, armLength, neckEndPoint);

    // Left arm: slightly offset by ±15 degrees
    float leftArmAngle = angle + (15.0f * M_PI / 180.0f);

    leftArmXPosition = _apply2DPhysics.getXForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, x);
    leftArmYPosition = _apply2DPhysics.getYForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, neckEndPoint);
}

// ---------------------------
// Drawing functions
// ---------------------------

void Player::renderHead(SDL_Renderer *r)
{
    SDL_RenderCircle(r, x, headYPosition, h / 7.0f);
}

void Player::renderNeck(SDL_Renderer *r)
{
    SDL_RenderLine(r, x, neckStartPoint, x, neckEndPoint);
}

void Player::renderBody(SDL_Renderer *r)
{
    SDL_RenderLine(r, x, neckEndPoint, x, bodyYEndPoint);
}

void Player::renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide)
{
    float x2 = rightSide ? baseX + angle : baseX - angle;
    SDL_RenderLine(r, baseX, baseY, x2, baseY + length);
}

void Player::render(SDL_Renderer *r)
{
    renderHead(r);
    renderNeck(r);
    renderBody(r);

    renderLimb(r, x, neckEndPoint, rightArmXPosition - x, rightArmYPosition - neckEndPoint, true);
    renderLimb(r, x, neckEndPoint, leftArmXPosition - x, leftArmYPosition - neckEndPoint, true);

    renderLimb(r, x, bodyYEndPoint, legsAngle, legsLength, false);
    renderLimb(r, x, bodyYEndPoint, legsAngle, legsLength, true);
}
