#include "Player.h"

Player::Player(const float screenW, const float screenH, const int SIZE)
    : _screenWidth(screenW), _screenHeight(screenH), _SIZE(SIZE)
{
    _playerPhysics.x0 = _screenWidth / 2.0f;
    _playerPhysics.y0 = _screenHeight / 2.0f;

    _playerPhysics.height = SIZE;
    _playerPhysics.width = SIZE / 2;
}

void Player::update(const float mx, const float my, const double deltaTime, const double GRAVITY, const int JUMP_COUNT, SDL_FRect screenBox)
{
    // 1️⃣ Update physics if not colliding

    _playerPhysics.totalTime += deltaTime;

    _playerPhysics.x = _applyPhysics.getFinalPosition(0, _playerPhysics.vx, _playerPhysics.x0, _playerPhysics.totalTime);
    _playerPhysics.y = _applyPhysics.getFinalPosition(GRAVITY, _playerPhysics.vy, _playerPhysics.y0, _playerPhysics.totalTime);

    // 2️⃣ Update hitbox from physics
    playerHitBoxObj.x = _playerPhysics.x - _playerPhysics.width / 2.0f;
    playerHitBoxObj.y = _playerPhysics.y - _playerPhysics.height / 2.0f;
    playerHitBoxObj.width = _playerPhysics.width;
    playerHitBoxObj.height = _playerPhysics.height;

    // 3️⃣ Check collisions (this may adjust the hitbox)
    _playerPhysics.collide.edgeCollision(playerHitBoxObj, screenBox);

    // 4️⃣ Sync physics center back to hitbox (if collision corrected)
    _playerPhysics.x = playerHitBoxObj.x + playerHitBoxObj.width / 2.0f;
    _playerPhysics.y = playerHitBoxObj.y + playerHitBoxObj.height / 2.0f;

    if (_playerPhysics.collide.collide)
    {
        // ✅ Standing on ground — reset vertical motion
        // _playerPhysics.vy = 0;
        _playerPhysics.y0 = _playerPhysics.y;
        _playerPhysics.totalTime = 0;
    }

    // 5️⃣ Refresh edge values for debugging or rendering
    top = _playerPhysics.y - _playerPhysics.height / 2;
    bottom = _playerPhysics.y + _playerPhysics.height / 2;
    right = _playerPhysics.x + _playerPhysics.width / 2;
    left = _playerPhysics.x - _playerPhysics.width / 2;

    playerHitBox = {left, top, _playerPhysics.width, _playerPhysics.height};

    if (_playerPhysics.collide.collideBottom)
        jumps = JUMP_COUNT;

    stand(mx, my);
}

void Player::stand(float mx, float my)
{
    headYPosition = _playerPhysics.y - _playerPhysics.height * 0.35f;
    neckStartPoint = headYPosition + _playerPhysics.height * 0.15f;
    neckEndPoint = neckStartPoint + _playerPhysics.height * 0.1f;
    bodyYEndPoint = neckEndPoint + _playerPhysics.height * 0.35f;
    armLength = _playerPhysics.height * 0.2f;
    legsAngle = 10;
    legsLength = armLength;

    _updateArmAngles(mx, my);
}

void Player::_updateArmAngles(float mx, float my)
{
    // Get base angle toward the mouse
    float angle = _applyPhysics.getVectorDirectionAngle(mx, my, _playerPhysics.x, neckEndPoint);

    // Right arm directly points toward the mouse
    rightArmXPosition = _applyPhysics.getXForVectorUsingAngleAndMagnitude(angle, armLength, _playerPhysics.x);
    rightArmYPosition = _applyPhysics.getYForVectorUsingAngleAndMagnitude(angle, armLength, neckEndPoint);

    // Left arm: slightly offset by ±15 degrees
    float leftArmAngle = angle + (15.0f * PI / 180.0f);

    leftArmXPosition = _applyPhysics.getXForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, _playerPhysics.x);
    leftArmYPosition = _applyPhysics.getYForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, neckEndPoint);
}

// ---------------------------
// Drawing functions
// ---------------------------

void Player::_renderHead(SDL_Renderer *r)
{
    SDL_RenderCircle(r, _playerPhysics.x, headYPosition, _SIZE / 7.0f);
}

void Player::_renderNeck(SDL_Renderer *r)
{
    SDL_RenderLine(r, _playerPhysics.x, neckStartPoint, _playerPhysics.x, neckEndPoint);
}

void Player::_renderBody(SDL_Renderer *r)
{
    SDL_RenderLine(r, _playerPhysics.x, neckEndPoint, _playerPhysics.x, bodyYEndPoint);
}

void Player::_renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide)
{
    float x2 = rightSide ? baseX + angle : baseX - angle;
    float y2 = baseY + length;

    SDL_RenderLine(r, baseX, baseY, x2, y2);
}

void Player::renderPlayerParts(SDL_Renderer *r)
{

    // 🟥 Head
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    _renderHead(r);

    // 🟨 Neck
    SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
    _renderNeck(r);

    // 🟩 Body
    SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
    _renderBody(r);

    // 🟦 Arms (both)
    SDL_SetRenderDrawColor(r, 0, 128, 255, 255);
    _renderLimb(r, _playerPhysics.x, neckEndPoint,
                rightArmXPosition - _playerPhysics.x,
                rightArmYPosition - neckEndPoint, true);
    _renderLimb(r, _playerPhysics.x, neckEndPoint,
                leftArmXPosition - _playerPhysics.x,
                leftArmYPosition - neckEndPoint, true);

    // 🟣 Legs (both)
    SDL_SetRenderDrawColor(r, 160, 32, 240, 255);
    _renderLimb(r, _playerPhysics.x, bodyYEndPoint, legsAngle, legsLength, false);
    _renderLimb(r, _playerPhysics.x, bodyYEndPoint, legsAngle, legsLength, true);
}
