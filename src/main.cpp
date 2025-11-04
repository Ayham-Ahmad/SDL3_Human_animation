#include "Game.h"

int main()
{
    Game game;
    game.init();

    game.NOW = SDL_GetPerformanceCounter(); 

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

    return 0;
}
