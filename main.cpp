#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "FPSTimer.h"

using namespace std;

const float SIZE = 200;
const float MAX_VERTICAL_FORCE = 20;
const float MAX_HORIZONTAL_FORCE = 20;

// Cache of precomputed circle points for each radius
static std::unordered_map<int, std::vector<SDL_FPoint>> circleCache;

// --- Drawing Player Parts --- //

void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
{
    int r = static_cast<int>(radius);

    // Check if circle points already cached
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

    // Draw using cached points
    const auto &points = circleCache[r];
    for (const auto &p : points)
        SDL_RenderPoint(renderer, cx + p.x, cy + p.y);
}

inline void head(SDL_Renderer *r, float x, float y)
{
    SDL_RenderCircle(r, x, y, SIZE / 7);
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

// --- Player Classes --- //

class Player
{
public:
    float screenWidth;
    float screenHeight;

    float X, Y;
    float H, W;
    float top, bottom, right, left;

    Player(float screenW, float screenH, float size = SIZE)
        : screenWidth(screenW), screenHeight(screenH), H(size), W(size / 2.0f)
    {
        X = screenWidth / 2.0f;
        Y = screenHeight / 2.0f;
        updateEdges();
    }

    void updateEdges()
    {
        top = Y - H / 2;
        bottom = Y + H / 2;
        right = X + W / 2;
        left = X - W / 2;
    }

    void setPosition(float x, float y)
    {
        X = x;
        Y = y;
        updateEdges();
    }
};

class Collide
{
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;

    void reset()
    {
        collideBottom = false;
        collideTop = false;
        collideRight = false;
        collideLeft = false;
    }
};

class ThrownDir
{
public:
    bool right = false;
    bool left = false;
};

// --- Utility Functions --- //

bool isCaught(float mx, float my, SDL_FRect box)
{
    return (mx >= box.x && mx <= box.x + box.w &&
            my >= box.y && my <= box.y + box.h);
}

void edgeCollision(float &HX, float &HY, const SDL_FRect &box, const SDL_FRect &sb, Collide &c)
{
    if (box.x + box.w > sb.w)
    {
        HX -= (box.x + box.w - sb.w);
        c.collideRight = true;
    }
    if (box.x < sb.x)
    {
        HX += (sb.x - box.x);
        c.collideLeft = true;
    }
    if (box.y < sb.y)
    {
        HY += (sb.y - box.y);
        c.collideTop = true;
    }
    if (box.y + box.h > sb.h)
    {
        HY -= (box.y + box.h - sb.h);
        c.collideBottom = true;
    }
}

float clamp(float value, float minVal, float maxVal)
{
    return (value < minVal) ? minVal : (value > maxVal) ? maxVal
                                                        : value;
}

// --- Motion Logic --- //

void fall(Player &player, float &vy, float gravity, ThrownDir &dir,
          Collide &col, bool &falling, float &vx, const SDL_FRect &box, const SDL_FRect &sb)
{
    vy += gravity;
    float remaining = sb.h - (box.y + box.h);

    if ((dir.right || dir.left) && !(col.collideLeft || col.collideRight))
        player.X += vx;
    else if (col.collideLeft || col.collideRight)
        vx = 0;

    if (col.collideTop)
        vy = fabs(vy);
    else if (remaining < vy)
    {
        vy = remaining;
        col.collideBottom = true;
    }

    player.Y += vy;

    if (col.collideBottom)
    {
        falling = false;
        vy = vx = 0;
        dir = ThrownDir();
    }
}

void throwDir(Uint32 &releaseTime, float &mouseX_after, float &mouseY, float lastX,
              float lastY, ThrownDir &dir, float &vx, float &vy, float gravity,
              bool &checkThrow, bool &falling)
{
    if (SDL_GetTicks() - releaseTime < 10)
        return;

    SDL_GetMouseState(&mouseX_after, &mouseY);
    float deltaX = mouseX_after - lastX;
    float deltaY = mouseY - lastY;

    dir.right = deltaX > 0;
    dir.left = deltaX < 0;

    vx = clamp(deltaX * gravity, -MAX_HORIZONTAL_FORCE, MAX_HORIZONTAL_FORCE);
    vy = clamp(deltaY * gravity, -MAX_VERTICAL_FORCE, MAX_VERTICAL_FORCE);

    checkThrow = false;
    falling = true;
}

// --- Drawing Human --- //

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

// --- Main Function --- //

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Human", 0, 0, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
    const int screenWidth = mode->w;
    const int screenHeight = mode->h;

    Player player(screenWidth, screenHeight);
    SDL_FRect hitbox = {player.left, player.top, player.W, player.H};

    bool running = true, caught = false, falling = true, checkThrowDir = false;
    float mouseX, mouseY, mouseX_after;
    float lastMouseX = player.X, lastMouseY = player.Y;

    float HY;      // Head Y position (center of the head)
    float NSP;     // Neck Start Point (where neck begins, below head)
    float NEP;     // Neck End Point (where neck ends, top of body)
    float HLL;     // Half Limb Length (used for arms and legs)
    float RHAngle; // Right Hand Angle (arm spread angle)
    float LHAngle; // Left Hand Angle
    float RLAngle; // Right Leg Angle
    float LLAngle; // Left Leg Angle
    float BEP;     // Body End Point (bottom of torso)

    stand(player, HY, NSP, NEP, HLL, RHAngle, LHAngle, RLAngle, LLAngle, BEP);

    float vx = 0.0f, vy = 0.0f, gravity = 0.5f;

    SDL_Event event;
    SDL_FRect screenBox = {0, 0, (float)screenWidth, (float)screenHeight};

    Collide collideChecker;
    ThrownDir thrownDir;

    Uint32 releaseTime = 0;

    Timer<60> timer; // 60 FPS limiter

    while (running)
    {
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                caught = isCaught(mouseX, mouseY, hitbox);
                if (caught)
                    falling = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
            {
                if (caught)
                {
                    caught = false;
                    checkThrowDir = true;
                    releaseTime = SDL_GetTicks();
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
            }
        }

        if (caught)
        {
            player.setPosition(mouseX, mouseY);
            vx = vy = 0;
        }

        if (checkThrowDir)
            throwDir(releaseTime, mouseX_after, mouseY, lastMouseX, lastMouseY, thrownDir, vx, vy, gravity, checkThrowDir, falling);

        if (falling)
            fall(player, vy, gravity, thrownDir, collideChecker, falling, vx, hitbox, screenBox);

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        collideChecker.reset();

        player.updateEdges();

        hitbox = {player.left, player.top, player.W, player.H};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderRect(renderer, &hitbox);

        edgeCollision(player.X, player.Y, hitbox, screenBox, collideChecker);

        stand(player, HY, NSP, NEP, HLL, RHAngle, LHAngle, RLAngle, LLAngle, BEP);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        getHuman(renderer, player, HY, NSP, NEP, HLL, RHAngle, LHAngle, BEP, RLAngle, LLAngle);

        SDL_RenderPresent(renderer);
        timer.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
