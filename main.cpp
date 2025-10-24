#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <iostream>

using namespace std;

const float SIZE = 35;
const float SPEED = 5;

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

// --- Body Functions --- //

float calculateA(float sec = 1)
{
    float a = 9.8 * sec;
    return (a * 0.01) * SPEED;
}

bool isCaught(float mx, float my, SDL_FRect box)
{
    return (mx >= box.x && mx <= box.x + box.w &&
            my >= box.y && my <= box.y + box.h);
}

SDL_FRect hitBox(SDL_Renderer *renderer, float HCP_X, float HCP_Y, float RHAngle, float LHAngle, float LEP)
{
    float padding = 3;

    float x = HCP_X - RHAngle - padding;
    float y = HCP_Y - SIZE - padding;
    float w = (LHAngle + RHAngle) + padding * 2;
    float h = LEP - y + padding;

    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_FRect box = {x, y, w, h};
    SDL_RenderRect(renderer, &box);

    return box;
}

// FIXED edgeCollide (works properly inside screen)
void edgeCollide(float &HCP_X, float &HCP_Y, SDL_FRect box, SDL_FRect sb)
{
    // Left edge
    if (box.x < sb.x)
        HCP_X += (sb.x - box.x);

    // Right edge
    if (box.x + box.w > sb.x + sb.w)
        HCP_X -= (box.x + box.w - (sb.x + sb.w));

    // Top edge
    if (box.y < sb.y)
        HCP_Y += (sb.y - box.y);

    // Bottom edge
    if (box.y + box.h > sb.y + sb.h)
        HCP_Y -= (box.y + box.h - (sb.y + sb.h));
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

    bool running = true, caught = false, falling = true, released = false;
    float mouseX, mouseY, lastMouseX, lastMouseY;
    bool debug = false;

    float HCP_X = screenWidth / 2;
    float HCP_Y = screenHeight / 2 - SIZE * 4;

    lastMouseX = HCP_X;
    lastMouseY = HCP_Y;

    float NSP = HCP_Y + SIZE;
    float NEP = HCP_Y + SIZE * 5;
    float HLL = HCP_Y + SIZE * 3;
    float RHAngle = 45, LHAngle = 45;
    float LSP = HCP_Y + SIZE * 5;
    float LEP = HLL + SIZE * 5;
    float RLAngle = 30, LLAngle = 30;

    SDL_Event event;
    SDL_FRect box;
    SDL_FRect screensb = {0, 0, (float)screenWidth, (float)screenHeight};

    Uint32 lastTime = SDL_GetTicks();
    int sec = 1;
    float a, sideA = 0.0f, afterReleasedSec = 0.0f;

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
                        falling = true;
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
        }

        // if (released)
        // {

        //     Uint32 currentTime = SDL_GetTicks();
        //     if (currentTime - lastTime >= 100 && released)
        //     {
        //         sideA = lastMouseX - mouseX;
        //         if (sideA > 0)
        //         sideA /= a;
        //         falling = true;
        //         released = false;
        //     }
        // }

        if (falling)
        {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastTime >= 1000)
            {
                sec++;
                lastTime = currentTime;
            }
            a = calculateA(sec);

            HCP_X = lastMouseX - sideA;
            HCP_Y = lastMouseY + a;
            lastMouseY = HCP_Y;
            if (HCP_Y >= screenHeight * 0.75)
                falling = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        box = hitBox(renderer, HCP_X, HCP_Y, RHAngle, LHAngle, LEP);
        edgeCollide(HCP_X, HCP_Y, box, screensb);

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
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
