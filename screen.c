#include <SDL2/SDL.h>
#include "screen.h"
#include "chip8.h"

SDL_Renderer* renderer;
SDL_Window* window;

void create_window()
{
    SDL_Init(SDL_INIT_VIDEO);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, ORIG_WIDTH, ORIG_HEIGHT);              

}

void draw_pixel()
{
    for (int i = 0; i < 32; ++i)
    {
        for (int j = 0; j < 64; ++j)
        {
            if (screen[i][j] == 1)
                SDL_SetRenderDrawColor(renderer, R_WHITE, G_WHITE, B_WHITE, OPAQUE);
            else
                SDL_SetRenderDrawColor(renderer, R_BLACK, G_BLACK, B_BLACK, OPAQUE);
            SDL_RenderDrawPoint(renderer, j, i);
        }
    }         
    SDL_RenderPresent(renderer);
}

void close_window() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}