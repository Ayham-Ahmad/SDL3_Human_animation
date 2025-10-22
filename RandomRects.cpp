#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Draw a Dot", 0, 0, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayID);

    // TTF_Font* font = TTF_OpenFont("C:\\Users\\Ayham\\Desktop\\Me\\code\\C++\\Projects\\TryAnimation\\Roboto-Medium.ttf", 32);

    // if (!font) {
    //     cerr << "Failed to load font: " << SDL_GetError() << endl;
    //     return 1;
    // }
    

    int screenWidth = mode->w;
    int screenHeight = mode->h;

    // Create a random device (seed source)
    random_device rd;
    mt19937 gen(rd()); // Mersenne Twister engine

    // Create a distribution range
    uniform_real_distribution<float> xDist(0, (float)screenWidth);
    uniform_real_distribution<float> yDist(0, (float)screenHeight);
    uniform_real_distribution<float> wDist((float)10, (float)200);
    uniform_real_distribution<float> hDist((float)10, (float)200);

    bool running = true;
    bool draw = true;
    SDL_Event event;

    // background black

    while (running) {
        // --- Handle Events ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (draw)
                        draw = false;
                    else
                        draw = true;
                }
            }
        }

    
        // --- Drawing --- //
         
        // Background
        SDL_SetRenderDrawColor(renderer, 0, 65, 99, 255); 
        SDL_RenderClear(renderer);


        if (draw) {
            for (int i = 0; i < 400; i++) {
                SDL_SetRenderDrawColor(renderer, rand() % 256, rand() % 256, rand() % 256, 255);
                SDL_FRect rect = {xDist(gen), yDist(gen), wDist(gen), hDist(gen)};
                SDL_RenderRect(renderer, &rect);
            }
            // --- Display ---
            SDL_RenderPresent(renderer);
        }

        // draw = false;

        SDL_Delay(300); 

    }   

    // --- Cleanup ---
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
