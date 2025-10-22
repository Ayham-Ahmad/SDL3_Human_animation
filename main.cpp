#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

const float SIZE = 35;

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

void head(SDL_Renderer *renderer, float BCPW, float BCPH, bool debug = false)
{
    if (debug)
        cout << "[head] BCPW: " << BCPW
             << " | BCPH: " << BCPH
             << " | SIZE: " << SIZE << endl;

    SDL_RenderCircle(renderer, BCPW, BCPH, SIZE);
}

void nick(SDL_Renderer *renderer, float BCPW, float NSP, float NEP, bool debug = false)
{
    if (debug)
        cout << "[nick] BCPW: " << BCPW
             << " | NSP: " << NSP
             << " | NEP: " << NEP << endl;

    SDL_RenderLine(renderer, BCPW, NSP, BCPW, NEP);
}

void Rhand(SDL_Renderer *renderer, float BCPW, float NSP, float RHAngle, float HLL, bool debug = false)
{
    if (debug)
        cout << "[Rhand] BCPW: " << BCPW
             << " | NSP: " << NSP
             << " | RHAngle: " << RHAngle
             << " | HLL: " << HLL << endl;

    SDL_RenderLine(renderer, BCPW, NSP, BCPW - RHAngle, HLL);
}

void Lhand(SDL_Renderer *renderer, float BCPW, float BCPH, float NSP, float LHAngle, float HLL, bool debug = false)
{
    if (debug)
        cout << "[Lhand] BCPW: " << BCPW
             << " | BCPH: " << BCPH
             << " | NSP: " << NSP
             << " | LHAngle: " << LHAngle
             << " | HLL: " << HLL << endl;

    SDL_RenderLine(renderer, BCPW, NSP, BCPW + LHAngle, HLL);
}

void RLeg(SDL_Renderer *renderer, float BCPW, float LSP, float RLAngle, float LEP, bool debug = false)
{
    if (debug)
        cout << "[RLeg] BCPW: " << BCPW
             << " | LSP: " << LSP
             << " | RLAngle: " << RLAngle
             << " | LEP: " << LEP << endl;

    SDL_RenderLine(renderer, BCPW, LSP, BCPW - RLAngle, LEP);
}

void LLeg(SDL_Renderer *renderer, float BCPW, float LSP, float LLAngle, float LEP, bool debug = false)
{
    if (debug)
        cout << "[LLeg] BCPW: " << BCPW
             << " | LSP: " << LSP
             << " | LLAngle: " << LLAngle
             << " | LEP: " << LEP << endl;

    SDL_RenderLine(renderer, BCPW, LSP, BCPW + LLAngle, LEP);
}

// --- Body Functions --- //

float calculateA(float sec = 1)
{
    float a = 9.8 * sec;
    // cout << "a: " << a << endl;
    return a * 0.1;
}

bool isCaught(float mx, float my, SDL_FRect box)
{
    return (mx >= box.x && mx <= box.x + box.w &&
            my >= box.y && my <= box.y + box.h);
}

SDL_FRect hitBox(SDL_Renderer *renderer, float BCPW, float BCPH, float RHAngle, float LHAngle, float LEP)
{
    float padding = 1;

    float x = BCPW - RHAngle - padding;
    float y = BCPH - SIZE - padding;
    float w = BCPW + LHAngle - x + padding;
    float h = LEP - y + padding;

    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_FRect box = {x, y, w, h};
    SDL_RenderRect(renderer, &box);

    return box;
}

// --- Set And Get Human --- //

void getHuman(SDL_Renderer *renderer, float BCPW, float BCPH, float NSP, float NEP, float HLL,
              float RHAngle, float LHAngle, float LSP, float LEP, float RLAngle, float LLAngle,
              bool debug = false)
{
    head(renderer, BCPW, BCPH, debug);
    nick(renderer, BCPW, NSP, NEP, debug);
    Rhand(renderer, BCPW, NSP, RHAngle, HLL, debug);
    Lhand(renderer, BCPW, BCPH, NSP, LHAngle, HLL, debug);
    RLeg(renderer, BCPW, LSP, RLAngle, LEP, debug);
    LLeg(renderer, BCPW, LSP, LLAngle, LEP, debug);
}

// --- Main Function --- //

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    //  Initialize Window & Renderer
    SDL_Window *window = SDL_CreateWindow("Human", 0, 0, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer)
    {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Extracting the screen dimensions
    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);

    const int screenWidth = mode->w;
    const int screenHeight = mode->h;

    // Init variables
    bool running = true;
    bool catchBody = false;
    bool releasedBody = false;

    float BCPW = screenWidth / 2;     // Body Center Point Width
    float BCPH = screenHeight * 0.75; // Body Center Point Height

    float NSP = BCPH + SIZE;     // Nick Start Point
    float NEP = BCPH + SIZE * 5; // Nick End Point

    float HLL = BCPH + SIZE * 3; // Hands & Legs Length

    float RHAngle = 45; // Right Hand Angle
    float LHAngle = 45; // Left Hand Angle

    float LSP = BCPH + SIZE * 5; // Legs Start Point
    float LEP = HLL + SIZE * 5;  // Legs End Point

    float RLAngle = 30; // Right Leg Angle
    float LLAngle = 30; // Left Leg Angle

    bool debug = false;

    float mouseX, mouseY;
    float lastMouseX, lastMouseY;

    Uint32 lastTime = SDL_GetTicks(); // Start time
    int sec = 1;
    float a = 0.0f;

    // Init objs
    SDL_Event event;
    SDL_FRect box;

    // Main loop
    while (running)
    {
        // Getting mouse x & y
        SDL_GetMouseState(&mouseX, &mouseY);

        // --- Handle Events ---
        while (SDL_PollEvent(&event))
        {
            // Close window
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;

            // Left click
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    // cout << "Down" << endl;
                    catchBody = isCaught(mouseX, mouseY, box);
                    if (catchBody)
                        releasedBody = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (catchBody)
                    {
                        catchBody = false;
                        releasedBody = true;
                        lastMouseX = mouseX;
                        lastMouseY = mouseY;
                        sec = 1;
                    }
                }
            }
        }

        if (catchBody)
        {
            BCPH = mouseY;
            BCPW = mouseX;
        }

        if (releasedBody)
        {
            // ---  Time --- //
            Uint32 currentTime = SDL_GetTicks();

            // check if 1 second (1000 ms) passed
            if (currentTime - lastTime >= 1000)
            {
                sec++;
                lastTime = currentTime; // reset the timer
            }

            a = calculateA(sec);
            BCPW = lastMouseX;
            BCPH = lastMouseY + a;

            lastMouseY = BCPH;

            if (BCPH >= screenHeight * 0.75)
                releasedBody = false;
        }

        NSP = BCPH + SIZE;     // Nick Start Point
        NEP = BCPH + SIZE * 5; // Nick End Point
        HLL = BCPH + SIZE * 3; // Hands & Legs Length
        RHAngle = 45;          // Right Hand Angle
        LHAngle = 45;          // Left Hand Angle
        LSP = BCPH + SIZE * 5; // Legs Start Point
        LEP = HLL + SIZE * 5;  // Legs End Point
        RLAngle = 30;          // Right Leg Angle
        LLAngle = 30;          // Left Leg Angle

        // --- Drawing --- //

        // Background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Human drawing
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // setHuman(mouseX, mouseY, lastMouseX, lastMouseY, catchBody, releasedBody, a);
        getHuman(renderer, BCPW, BCPH, NSP, NEP, HLL, RHAngle, LHAngle, LSP, LEP, RLAngle, LLAngle, debug);
        box = hitBox(renderer, BCPW, BCPH, RHAngle, LHAngle, LEP);

        // Display
        SDL_RenderPresent(renderer);
    }

    // --- Cleanup ---
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// don't let the body go beyond the screen
// make it fall slightly in the way of throwing