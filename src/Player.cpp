#include "Player.h"

Player::Player(const float screenW, const float screenH)
    : _screenWidth(screenW), _screenHeight(screenH)
{
    _Rigidbody.x0 = _screenWidth / 2.0f;
    _Rigidbody.y0 = _screenHeight / 2.0f;

    _Rigidbody.height = SIZE;
    _Rigidbody.width = SIZE / 2;

    _inAir = true;

    _Rigidbody.x = _applyPhysics.getFinalPosition(0, _Rigidbody.vx, _Rigidbody.x0, _Rigidbody.totalTime);
    _Rigidbody.y = _applyPhysics.getFinalPosition(GRAVITY, _Rigidbody.vy, _Rigidbody.y0, _Rigidbody.totalTime);
}

void Player::update(const float mx, const float my, SDL_FRect screenBox)
{
    // Apply gravity and movement
    if (_Rigidbody.affectedByGravity)
        _Rigidbody.vy += GRAVITY * deltaTime; // accumulate velocity

    // update position
    _applyPhysics.updatePositionXBasedOnVXAndDeltaTime(_Rigidbody);
    _applyPhysics.updatePositionYBasedOnVYAndDeltaTime(_Rigidbody);

    // Update hitbox
    _updateHitboxFromRigidbody();

    // Check collisions
    _Rigidbody.collide.edgeCollision(playerHitBoxRigidbody, screenBox);

    // Sync physics center after collision
    _syncRigidbodyWithHitbox();

    // Handle Collision
    _handleCollisionResponse();

    // Update edges for rendering/debugging
    _updateHitboxEdges();

    _updateStandingState();

    _updateArmAngles(mx, my);
}

void Player::jump()
{
    if (_jumps <= JUMP_COUNT && _jumps != 0)
    {
        _Rigidbody.vy = -sqrt(2 * GRAVITY * JUMP_HEIGHT);
        _jumps -= 1;

        _inAir = true;
    }
}

void Player::handleHorizontalMovement(const bool *state)
{
    const float dt = static_cast<float>(deltaTime);

    constexpr float AIR_ACCEL = 2000.0f;
    constexpr float GROUND_ACCEL = 4000.0f;

    const float ACCEL = _inAir ? AIR_ACCEL : GROUND_ACCEL;

    if ((state[SDL_SCANCODE_A] && _Rigidbody.collide.collideLeft) ||
        (state[SDL_SCANCODE_D] && _Rigidbody.collide.collideRight))
    {
        _handleWallSlide();
    }
    else if (state[SDL_SCANCODE_A])
        _Rigidbody.vx = std::max(_Rigidbody.vx - ACCEL * dt, -MOVEMENT_SPEED);
    else if (state[SDL_SCANCODE_D])
        _Rigidbody.vx = std::min(_Rigidbody.vx + ACCEL * dt, MOVEMENT_SPEED);
    else
    {
        // Decelerate smoothly
        if (_Rigidbody.vx > 0)
            _Rigidbody.vx = std::max(0.0f, _Rigidbody.vx - ACCEL * dt);
        else
            _Rigidbody.vx = std::min(0.0f, _Rigidbody.vx + ACCEL * dt);
    }
}

void Player::_handleWallSlide()
{
    static double wallSlideElapsed = 0.0;

    // Slow down fall while sliding
    if (_Rigidbody.vy > 2.0f)
        _Rigidbody.vy = 2.0f;

    // Regain jump after sliding for 1.5s
    if (_jumps == 0)
    {
        wallSlideElapsed += deltaTime;

        if (wallSlideElapsed >= 1.5) // 1.5 seconds
        {
            _jumps += 1;
            wallSlideElapsed = 0.0;
        }
    }
    else
    {
        wallSlideElapsed = 0.0;
    }
}

void Player::_updateStandingState()
{
    headYPosition = _Rigidbody.y - _Rigidbody.height * 0.35f;
    neckStartPoint = headYPosition + _Rigidbody.height * 0.15f;
    neckEndPoint = neckStartPoint + _Rigidbody.height * 0.1f;
    bodyYEndPoint = neckEndPoint + _Rigidbody.height * 0.35f;
    armLength = _Rigidbody.height * 0.2f;
    legsAngle = 10;
    legsLength = armLength;
}

void Player::_updateArmAngles(float mx, float my)
{
    // Get base angle toward the mouse
    float angle = _applyPhysics.getVectorDirectionAngle(mx, my, _Rigidbody.x, neckEndPoint);

    // Right arm directly points toward the mouse
    rightArmXPosition = _applyPhysics.getXForVectorUsingAngleAndMagnitude(angle, armLength, _Rigidbody.x);
    rightArmYPosition = _applyPhysics.getYForVectorUsingAngleAndMagnitude(angle, armLength, neckEndPoint);

    // Left arm: slightly offset by ±15 degrees
    float leftArmAngle = angle + (15.0f * PI / 180.0f);

    leftArmXPosition = _applyPhysics.getXForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, _Rigidbody.x);
    leftArmYPosition = _applyPhysics.getYForVectorUsingAngleAndMagnitude(leftArmAngle, armLength, neckEndPoint);
}

void Player::_updateHitboxFromRigidbody()
{
    playerHitBoxRigidbody.x = _Rigidbody.x - _Rigidbody.width / 2.0f;
    playerHitBoxRigidbody.y = _Rigidbody.y - _Rigidbody.height / 2.0f;
    playerHitBoxRigidbody.width = _Rigidbody.width;
    playerHitBoxRigidbody.height = _Rigidbody.height;
}

void Player::_syncRigidbodyWithHitbox()
{
    _Rigidbody.x = playerHitBoxRigidbody.x + playerHitBoxRigidbody.width / 2.0f;
    _Rigidbody.y = playerHitBoxRigidbody.y + playerHitBoxRigidbody.height / 2.0f;
}

void Player::_handleCollisionResponse()
{
    if (_Rigidbody.collide.collideBottom)
    {
        _Rigidbody.vy = 0;   // stop vertical movement
        _jumps = JUMP_COUNT; // reset jump counter
        _inAir = false;

        _Rigidbody.y0 = _Rigidbody.y;
        _Rigidbody.totalTime = 0;
    }

    if (_Rigidbody.collide.collideRight || _Rigidbody.collide.collideLeft)
    {
        _Rigidbody.vx = 0;
    }
}

void Player::_updateHitboxEdges()
{
    top = _Rigidbody.y - _Rigidbody.height / 2;
    bottom = _Rigidbody.y + _Rigidbody.height / 2;
    right = _Rigidbody.x + _Rigidbody.width / 2;
    left = _Rigidbody.x - _Rigidbody.width / 2;

    playerHitBox = {left, top, _Rigidbody.width, _Rigidbody.height};
}

// ---------------------------
// Drawing functions
// ---------------------------

void Player::_renderHead(SDL_Renderer *r)
{
    SDL_RenderCircle(r, _Rigidbody.x, headYPosition, SIZE / 7.0f);
}

void Player::_renderNeck(SDL_Renderer *r)
{
    SDL_RenderLine(r, _Rigidbody.x, neckStartPoint, _Rigidbody.x, neckEndPoint);
}

void Player::_renderBody(SDL_Renderer *r)
{
    SDL_RenderLine(r, _Rigidbody.x, neckEndPoint, _Rigidbody.x, bodyYEndPoint);
}

void Player::_renderLimb(SDL_Renderer *r, float baseX, float baseY, float angle, float length, bool rightSide)
{
    float x2 = rightSide ? baseX + angle : baseX - angle;
    float y2 = baseY + length;

    SDL_RenderLine(r, baseX, baseY, x2, y2);
}

void Player::renderPlayerParts(SDL_Renderer *r)
{

    // Head
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    _renderHead(r);

    // Neck
    SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
    _renderNeck(r);

    // Body
    SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
    _renderBody(r);

    // Arms (both)
    SDL_SetRenderDrawColor(r, 0, 128, 255, 255);
    _renderLimb(r, _Rigidbody.x, neckEndPoint,
                rightArmXPosition - _Rigidbody.x,
                rightArmYPosition - neckEndPoint, true);
    _renderLimb(r, _Rigidbody.x, neckEndPoint,
                leftArmXPosition - _Rigidbody.x,
                leftArmYPosition - neckEndPoint, true);

    // Legs (both)
    SDL_SetRenderDrawColor(r, 160, 32, 240, 255);
    _renderLimb(r, _Rigidbody.x, bodyYEndPoint, legsAngle, legsLength, false);
    _renderLimb(r, _Rigidbody.x, bodyYEndPoint, legsAngle, legsLength, true);
}
