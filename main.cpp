#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <iostream>
#include "FPSTimer.h"

using namespace std;

const float SIZE = 35;
const float SPEED = 1;
const float PADDING = 1;
const float MAX_VERTICAL_SPEED = 20;
const float MAX_HORIZONTAL_SPEED = 20;
// const float BODYHEIGHT = SIZE * 5; // From the HCP_Y to LEP

// --- Drawing Body Parts --- //

void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius)
{
    int x = radius;
    int y = 0;
    int decision = 1 - x;

    while (y <= x)
    {
        SDL_RenderPoint(renderer, cx + x, cy + y);
        SDL_RenderPoint(renderer, cx + y, cy + x);
        SDL_RenderPoint(renderer, cx - y, cy + x);
        SDL_RenderPoint(renderer, cx - x, cy + y);
        SDL_RenderPoint(renderer, cx - x, cy - y);
        SDL_RenderPoint(renderer, cx - y, cy - x);
        SDL_RenderPoint(renderer, cx + y, cy - x);
        SDL_RenderPoint(renderer, cx + x, cy - y);

        y++;
        if (decision <= 0)
            decision += 2 * y + 1;
        else
        {
            x--;
            decision += 2 * (y - x) + 1;
        }
    }
}

void head(SDL_Renderer *renderer, float HCP_X, float HCP_Y, bool debug = false)
{
    if (debug)
        cout << "[head] HCP_X: " << HCP_X
             << " | HCP_Y: " << HCP_Y
             << " | SIZE: " << SIZE << endl;

    SDL_RenderCircle(renderer, HCP_X, HCP_Y, SIZE);
}

void nick(SDL_Renderer *renderer, float HCP_X, float NSP, float NEP, bool debug = false)
{
    if (debug)
        cout << "[nick] HCP_X: " << HCP_X
             << " | NSP: " << NSP
             << " | NEP: " << NEP << endl;

    SDL_RenderLine(renderer, HCP_X, NSP, HCP_X, NEP);
}

void Rhand(SDL_Renderer *renderer, float HCP_X, float NSP, float RHAngle, float HLL, bool debug = false)
{
    if (debug)
        cout << "[Rhand] HCP_X: " << HCP_X
             << " | NSP: " << NSP
             << " | RHAngle: " << RHAngle
             << " | HLL: " << HLL << endl;

    SDL_RenderLine(renderer, HCP_X, NSP, HCP_X - RHAngle, HLL);
}

void Lhand(SDL_Renderer *renderer, float HCP_X, float HCP_Y, float NSP, float LHAngle, float HLL, bool debug = false)
{
    if (debug)
        cout << "[Lhand] HCP_X: " << HCP_X
             << " | HCP_Y: " << HCP_Y
             << " | NSP: " << NSP
             << " | LHAngle: " << LHAngle
             << " | HLL: " << HLL << endl;

    SDL_RenderLine(renderer, HCP_X, NSP, HCP_X + LHAngle, HLL);
}

void RLeg(SDL_Renderer *renderer, float HCP_X, float LSP, float RLAngle, float LEP, bool debug = false)
{
    if (debug)
        cout << "[RLeg] HCP_X: " << HCP_X
             << " | LSP: " << LSP
             << " | RLAngle: " << RLAngle
             << " | LEP: " << LEP << endl;

    SDL_RenderLine(renderer, HCP_X, LSP, HCP_X - RLAngle, LEP);
}

void LLeg(SDL_Renderer *renderer, float HCP_X, float LSP, float LLAngle, float LEP, bool debug = false)
{
    if (debug)
        cout << "[LLeg] HCP_X: " << HCP_X
             << " | LSP: " << LSP
             << " | LLAngle: " << LLAngle
             << " | LEP: " << LEP << endl;

    SDL_RenderLine(renderer, HCP_X, LSP, HCP_X + LLAngle, LEP);
}

// --- Body Classes --- //

class Collide
{
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;
};

class ThrownDir
{
public:
    bool right, left = false;
};
// --- Body Functions --- //

float calculateAcceleration(float sec = 1)
{
    float a = 9.8 * sec;
    return (a * 0.01) * SPEED;
}

bool isCaught(float mx, float my, SDL_FRect box)
{
    return (mx >= box.x && mx <= box.x + box.w &&
            my >= box.y && my <= box.y + box.h);
}

SDL_FRect hitBox(SDL_Renderer *renderer, float HCP_X, float HCP_Y, float RHAngle, float LHAngle, float LEP, bool showHitBox = false)
{
    float x = HCP_X - RHAngle - PADDING;
    float y = HCP_Y - SIZE - PADDING;
    float w = (LHAngle + RHAngle) + PADDING * 2;
    float h = LEP - y + PADDING;

    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_FRect box = {x, y, w, h};

    if (showHitBox)
        SDL_RenderRect(renderer, &box);

    return box;
}

void edgeCollide(float &HCP_X, float &HCP_Y, SDL_FRect box, SDL_FRect sb, Collide &collideChecker)
{

    // Right edge
    if (box.x + box.w > sb.x + sb.w)
    {
        HCP_X -= (box.x + box.w - (sb.x + sb.w));
        collideChecker.collideRight = true;
    }

    // Left edge
    if (box.x < sb.x)
    {
        HCP_X += (sb.x - box.x);
        collideChecker.collideLeft = true;
    }

    // Top edge
    if (box.y < sb.y)
    {
        HCP_Y += (sb.y - box.y);
        collideChecker.collideTop = true;
    }

    // Bottom edge
    if (box.y + box.h > sb.y + sb.h)
    {
        HCP_Y -= (box.y + box.h - (sb.y + sb.h));
        cout << "Inside edgeCollide: HCP_Y = " << HCP_Y << endl;
        collideChecker.collideBottom = true;
    }
}

float clamp(float value, float minVal, float maxVal)
{
    if (value < minVal)
        return minVal;
    if (value > maxVal)
        return maxVal;
    return value;
}
// --- Set And Get Human --- //

void getHuman(SDL_Renderer *renderer, float HCP_X, float HCP_Y, float NSP, float NEP, float HLL,
              float RHAngle, float LHAngle, float LSP, float LEP, float RLAngle, float LLAngle,
              bool debug = false)
{
    head(renderer, HCP_X, HCP_Y, debug);
    nick(renderer, HCP_X, NSP, NEP, debug);
    Rhand(renderer, HCP_X, NSP, RHAngle, HLL, debug);
    Lhand(renderer, HCP_X, HCP_Y, NSP, LHAngle, HLL, debug);
    RLeg(renderer, HCP_X, LSP, RLAngle, LEP, debug);
    LLeg(renderer, HCP_X, LSP, LLAngle, LEP, debug);
}

// --- Main Function --- //

int main(int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
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

    bool running = true, caught = false, falling = true, checkThrowDir = false;
    float mouseX, mouseY, lastMouseX, lastMouseY, mouseX_after100ms, sideCollideMouseX;
    bool debug = false, showHitBox = false;

    float HCP_X = screenWidth / 2;
    float HCP_Y = screenHeight / 2 - SIZE * 4;

    cout << "Outside the main: HCP_Y = " << HCP_Y << endl;

    lastMouseX = HCP_X;
    lastMouseY = HCP_Y;

    float NSP = HCP_Y + SIZE;
    float NEP = HCP_Y + SIZE * 5;
    float HLL = HCP_Y + SIZE * 3;
    float RHAngle = 45, LHAngle = 45;
    float LSP = HCP_Y + SIZE * 5;
    float LEP = HLL + SIZE * 5;
    float RLAngle = 30, LLAngle = 30;

    float vx = 0.0f, vy = 0.0f;
    float gravity = 0.5f;

    SDL_Event event;
    SDL_FRect box;
    SDL_FRect screensb = {0, 0, (float)screenWidth, (float)screenHeight};
    Collide collideChecker;
    ThrownDir thrownDir;

    Uint32 lastTime, releaseTime, lastUpdate = SDL_GetTicks();

    int sec = 1;
    float a, sideA = 0.0f;

    Timer<60> timer; // for 60 FPS

    while (running)
    {
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    caught = isCaught(mouseX, mouseY, box);
                    if (caught)
                        falling = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (caught)
                    {
                        caught = false;
                        checkThrowDir = true;

                        lastMouseX = mouseX;
                        lastMouseY = mouseY;
                        sec = 1;
                    }
                }
            }
        }

        if (caught)
        {
            HCP_Y = mouseY;
            HCP_X = mouseX;
            vx = vy = 0;
        }

        if (checkThrowDir)
        {
            Uint32 now = SDL_GetTicks();
            if (now - releaseTime >= 10) // 100ms passed
            {
                SDL_GetMouseState(&mouseX_after100ms, &mouseY);

                float deltaX = mouseX_after100ms - lastMouseX;
                // cout << mouseX_after100ms << " - " << lastMouseX << " = " << deltaX << endl;

                if (deltaX > 0)
                {
                    thrownDir.right = true;
                    cout << "Thrown to the RIGHT\n";
                }
                else if (deltaX < 0)
                {
                    thrownDir.left = true;
                    cout << "Thrown to the LEFT\n";
                }
                else
                    thrownDir = ThrownDir();

                // Compute throw velocity based on recent mouse movement
                vx = deltaX * gravity;                // horizontal velocity
                vy = (mouseY - lastMouseY) * gravity; // vertical velocity
                cout << vx << " " << vy << endl;

                vx = clamp(vx, -MAX_HORIZONTAL_SPEED, MAX_HORIZONTAL_SPEED);
                vy = clamp(vy, -MAX_VERTICAL_SPEED, MAX_VERTICAL_SPEED);
                cout << vx << " " << vy << endl;

                checkThrowDir = false;
                falling = true;
                sideA = deltaX / 100;
                // cout << sideA << endl;
            }
        }

        if (falling)
        {
            vy += gravity; // apply gravity

            // horizontal motion
            if ((thrownDir.right || thrownDir.left) & !(collideChecker.collideLeft || collideChecker.collideRight))
            {
                HCP_X += vx;
            }
            else if (collideChecker.collideLeft || collideChecker.collideRight)
                vx = 0;
            else
                HCP_X = lastMouseX;

            // vertical motion
            if (collideChecker.collideTop)
                vy = abs(vy);
            HCP_Y += vy;

            cout << "Inside falling: HCP_Y = " << HCP_Y << " " << vy << endl;

            // Collision with ground
            if (collideChecker.collideBottom)
            {
                falling = false;
                vy = 0;
                vx = 0;
                thrownDir = ThrownDir();
                cout << "floor" << endl;
            }
        }

        // cout << HCP_X << endl;
        // cout << HCP_Y << endl;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        collideChecker = Collide();

        box = hitBox(renderer, HCP_X, HCP_Y, RHAngle, LHAngle, LEP);
        edgeCollide(HCP_X, HCP_Y, box, screensb, collideChecker);

        NSP = HCP_Y + SIZE;
        NEP = HCP_Y + SIZE * 5;
        HLL = HCP_Y + SIZE * 3;
        LSP = HCP_Y + SIZE * 5;
        LEP = HLL + SIZE * 5;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &screensb);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        getHuman(renderer, HCP_X, HCP_Y, NSP, NEP, HLL, RHAngle, LHAngle, LSP, LEP, RLAngle, LLAngle, debug);

        SDL_RenderPresent(renderer);

        timer.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
