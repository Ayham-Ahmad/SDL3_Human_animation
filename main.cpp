#include <SDL3/SDL_main.h>
#include <vector>
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

    game.player = Player(screenWidth, screenHeight);
    game.playerHitBox = {game.player.left, game.player.top, game.player.playerStatus.width, game.player.playerStatus.height};
    game.screenBox = {0, 0, (float)screenWidth, (float)screenHeight};

    game.lastMouseX = game.player.playerStatus.x0;
    game.lastMouseY = game.player.playerStatus.y0;

    // Object player;
    // Physics2D applyPhysics;
    // SDL_FRect playerRect;

    while (game.running)
    {
        SDL_GetMouseState(&game.mouseX, &game.mouseY);

        game.LAST = game.NOW;
        game.NOW = SDL_GetPerformanceCounter();
        game.deltaTime = (double)((game.NOW - game.LAST) / (double)SDL_GetPerformanceFrequency());

        // if (!player.collide.collideBottom)
        //     player.totalTime += game.deltaTime;
        // else
        // {
        //     player.x = playerRect.x;
        //     player.y = playerRect.y;
        // }

        game.handleInput();
        game.update();
        game.render();

        // print(player.x, player.y, playerRect.x, playerRect.y, player.totalTime);

        // player.x = applyPhysics.getFinalPosition(0, player.vx0, player.x0, player.totalTime);
        // player.y = applyPhysics.getFinalPosition(GRAVITY, player.vy0, player.y0, player.totalTime);

        // player.collide.edgeCollision(player, game.screenBox);

        // playerRect = {player.x, player.y, player.width, player.height};

        // SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 0);
        // SDL_RenderClear(game.renderer);

        // SDL_SetRenderDrawColor(game.renderer, 128, 128, 128, 255);
        // SDL_RenderFillRect(game.renderer, &playerRect);

        // SDL_RenderPresent(game.renderer);

        game.timer.sleep();
    }

    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    SDL_Quit();

    return 0;
}
