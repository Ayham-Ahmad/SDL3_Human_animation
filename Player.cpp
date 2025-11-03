#include "Player.h"
#include "Game.h"
#include <cmath>
#include <unordered_map>
#include <vector>

static std::unordered_map<int, std::vector<SDL_FPoint>> circleCache;

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

Player::Player(float screenW, float screenH)
    : screenWidth(screenW), screenHeight(screenH)
{
    playerStatus.x0 = screenWidth / 2.0f;
    playerStatus.y0 = screenHeight / 2.0f;

    playerStatus.height = SIZE;
    playerStatus.width = SIZE / 2;

    update();
}

void Player::update()
{
    top = playerStatus.y - playerStatus.height / 2;
    bottom = playerStatus.y + playerStatus.height / 2;
    right = playerStatus.x + playerStatus.width / 2;
    left = playerStatus.x - playerStatus.width / 2;
}

void Player::stand(float mx, float my)
{
    headYPosition = playerStatus.y - playerStatus.height * 0.35f;
    neckStartPoint = headYPosition + playerStatus.height * 0.15f;
    neckEndPoint = neckStartPoint + playerStatus.height * 0.1f;
    bodyYEndPoint = neckEndPoint + playerStatus.height * 0.35f;
    armLength = playerStatus.height * 0.2f;
    legsAngle = 30;
    legsLength = armLength + 8;

    updateArmAngles(mx, my);
}

void Player::updateArmAngles(float mx, float my)
{
    // Get base angle toward the mouse
    float angle = _apply2DPhysics.getVectorDirectionAngle(mx, my, playerStatus.x, neckEndPoint);

    // Right arm directly points toward the mouse
    rightArmXPosition = _apply2DPhysics.getXForVectorUsingAngleAndMagnitude(angle, armLength, playerStatus.x);
    rightArmYPosition = _apply2DPhysics.getYForVectorUsingAngleAndMagnitude(angle, armLength, neckEndPoint);

    // Left arm: slightly offset by ±15 degrees
    float leftArmAngle = angle + (15.0f * PI / 180.0f);

    leftArmXPosition = _apply2DPhysics.getXForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, playerStatus.x);
    leftArmYPosition = _apply2DPhysics.getYForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, neckEndPoint);
}

// ---------------------------
// Drawing functions
// ---------------------------

void Player::renderHead(SDL_Renderer *r)
{
    SDL_RenderCircle(r, playerStatus.x, headYPosition, SIZE / 7.0f);
}

void Player::renderNeck(SDL_Renderer *r)
{
    SDL_RenderLine(r, playerStatus.x, neckStartPoint, playerStatus.x, neckEndPoint);
}

void Player::renderBody(SDL_Renderer *r)
{
    SDL_RenderLine(r, playerStatus.x, neckEndPoint, playerStatus.x, bodyYEndPoint);
}

void Player::renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide)
{
    float x2 = rightSide ? baseX + angle : baseX - angle;
    SDL_RenderLine(r, baseX, baseY, x2, baseY + length);
}

void Player::renderPlayerParts(SDL_Renderer *r)
{
    renderHead(r);
    renderNeck(r);
    renderBody(r);

    renderLimb(r, playerStatus.x, neckEndPoint, rightArmXPosition - playerStatus.x, rightArmYPosition - neckEndPoint, true);
    renderLimb(r, playerStatus.x, neckEndPoint, leftArmXPosition - playerStatus.x, leftArmYPosition - neckEndPoint, true);

    renderLimb(r, playerStatus.x, bodyYEndPoint, legsAngle, legsLength, false);
    renderLimb(r, playerStatus.x, bodyYEndPoint, legsAngle, legsLength, true);
}
