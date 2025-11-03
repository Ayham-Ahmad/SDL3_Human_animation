#include <SDL3/SDL_main.h>
#include "Game.h"

int SDL_main(int argc, char *argv[])
{
    Game game;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    game.window = SDL_CreateWindow("Human", 0, 0, SDL_WINDOW_FULLSCREEN);
    game.renderer = SDL_CreateRenderer(game.window, NULL);

    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
    const int screenWidth = mode->w;
    const int screenHeight = mode->h;

    game.player = Player(screenWidth, screenHeight, GRAVITY, SIZE);
    game.hitbox = {game.player.left, game.player.top, game.player.w, game.player.h};
    game.screenBox = {0, 0, (float)screenWidth, (float)screenHeight};

    game.lastMouseX = game.player.x;
    game.lastMouseY = game.player.y;

    while (game.running)
    {
        SDL_GetMouseState(&game.mouseX, &game.mouseY);

        game.LAST = game.NOW;
        game.NOW = SDL_GetPerformanceCounter();
        game.deltaTime = (double)((game.NOW - game.LAST) / (double)SDL_GetPerformanceFrequency());

        game.handleInput();
        game.update();
        game.render();

        game.timer.sleep();
    }

    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    SDL_Quit();

    return 0;
}
