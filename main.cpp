#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// #include <SDL3/SDL_ttf.h>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

const float SIZE = 35;

class Human
{

    SDL_Renderer *renderer;

    int sw = 0; // screen width
    int sh = 0; // screen height

    float BCPW = 0; // Body Center Point Width
    float BCPH = 0; // Body Center Point Height

    float NSP = 0; // Nick Start Point
    float NEP = 0; // Nick End Point

    float HLL = 0; // Hands & Legs Length

    float RHAngle = 0; // Right Hand Angle
    float LHAngle = 0; // Left Hand Angle

    float LSP = 0; // Legs Start Point
    float LEP = 0; // Legs End Point

    float RLAngle = 0; // Right Leg Angle
    float LLAngle = 0; // Left Leg Angle

    bool debug = false;

public:
    Human(SDL_Renderer *renderer, int screenWidth, int screenHeight)
    {
        this->renderer = renderer;
        this->sw = screenWidth;
        this->sh = screenHeight;
    }

private:
    void SDL_RenderCircle(int cx, int cy, int radius)
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

    void head()
    {
        if (debug)
            cout << "[head] BCPW: " << BCPW
                 << " | BCPH: " << BCPH
                 << " | SIZE: " << SIZE << endl;

        SDL_RenderCircle(BCPW, BCPH, SIZE);
    }

    void nick()
    {
        if (debug)
            cout << "[nick] BCPW: " << BCPW
                 << " | NSP: " << NSP
                 << " | NEP: " << NEP << endl;

        SDL_RenderLine(renderer, BCPW, NSP, BCPW, NEP);
    }

    void Rhand()
    {
        if (debug)
            cout << "[Rhand] BCPW: " << BCPW
                 << " | NSP: " << NSP
                 << " | RHAngle: " << RHAngle
                 << " | HLL: " << HLL << endl;

        SDL_RenderLine(renderer, BCPW, NSP, BCPW - RHAngle, HLL);
    }

    void Lhand()
    {
        if (debug)
            cout << "[Lhand] BCPW: " << BCPW
                 << " | BCPH: " << BCPH
                 << " | SIZE: " << SIZE
                 << " | LHAngle: " << LHAngle
                 << " | HLL: " << HLL << endl;

        SDL_RenderLine(renderer, BCPW, NSP, BCPW + LHAngle, HLL);
    }

    void RLeg()
    {
        if (debug)
            cout << "[RLeg] BCPW: " << BCPW
                 << " | LSP: " << LSP
                 << " | RLAngle: " << RLAngle
                 << " | LEP: " << LEP << endl;

        SDL_RenderLine(renderer, BCPW, LSP, BCPW - RLAngle, LEP);
    }

    void LLeg()
    {
        if (debug)
            cout << "[LLeg] BCPW: " << BCPW
                 << " | LSP: " << LSP
                 << " | LLAngle: " << LLAngle
                 << " | LEP: " << LEP << endl;

        SDL_RenderLine(renderer, BCPW, LSP, BCPW + LLAngle, LEP);
    }

public:
    void setHuman(int mouseX, int mouseY, bool catchBody, bool releasedBody, float a, bool debug = false)
    {

        cout << "mouseY + a" << mouseY + a << endl;
        if (catchBody)
        {
            this->BCPH = mouseY;
            this->BCPW = mouseX;
        }
        else if (releasedBody)
            this->BCPH = mouseY + a * 4;
        else
        {
            this->BCPH = sh * 0.75;
            this->BCPW = sw / 2;
        }

        this->NSP = BCPH + SIZE;
        this->NEP = BCPH + SIZE * 5;

        this->HLL = BCPH + SIZE * 3;

        this->RHAngle = 45;
        this->LHAngle = 45;

        this->LSP = BCPH + SIZE * 5;
        this->LEP = HLL + SIZE * 5;

        this->RLAngle = 30;
        this->LLAngle = 30;

        this->debug = debug;

        if (debug)
            cout << "\n\n--------------\n\n"
                 << endl;
    }

    void getHuman()
    {
        head();
        nick();
        Rhand();
        Lhand();
        RLeg();
        LLeg();
    }
};

bool isCaught(int mouseX, int mouseY, int sw, int sh)
{
    if (mouseY >= sh * 0.75 && mouseY <= sh)
        return true;
    return false;
}

float falling(int mouseY, int sh, int sec = 1)
{
    float a = 9.8 * sec;
    cout << "a: " << a << endl;
    return a;
}

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

    float mouseX, mouseY;
    float lastMouseX, lastMouseY;

    Uint32 lastTime = SDL_GetTicks(); // Start time
    int sec = 1;
    float a = 0.0f;

    // Init objs
    SDL_Event event;
    Human human(renderer, screenWidth, screenHeight);

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

            // Mouse wheel
            // else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            //     if (event.wheel.y > 0 && SIZE < 35) SIZE += 5;
            //     else if ( event.wheel.y < 0 && SIZE > 15) SIZE -=5;
            // }

            // Left click
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    cout << "Down" << endl;
                    catchBody = isCaught(mouseX, mouseY, screenWidth, screenHeight);
                    if (catchBody)
                        releasedBody = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    cout << "Up" << endl;
                    catchBody = false;
                    releasedBody = true;
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
            }
        }

        // ---  Time --- //
        Uint32 currentTime = SDL_GetTicks();

        // check if 1 second (1000 ms) passed
        if (currentTime - lastTime >= 1000)
        {
            sec++;
            lastTime = currentTime; // reset the timer
        }

        if (releasedBody)
        {
            a = falling(mouseY, screenHeight, sec);
            // if (a >= 20)
            //     break;
        }

        // --- Drawing --- //

        // Background
        SDL_SetRenderDrawColor(renderer, 0, 65, 99, 255);
        SDL_RenderClear(renderer);

        // Human drawing
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        human.setHuman(mouseX, mouseY, catchBody, releasedBody, a);
        human.getHuman();

        // Display
        SDL_RenderPresent(renderer);
    }

    // --- Cleanup ---
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// in falling: make last mouseY and mouseX
