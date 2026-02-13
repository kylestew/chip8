#include "graphics.h"
#include <stdio.h>

int graphics_init(Graphics *g) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    g->window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GFX_WIDTH * GFX_SCALE, GFX_HEIGHT * GFX_SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!g->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED);
    if (!g->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(g->window);
        SDL_Quit();
        return -1;
    }

    return 0;
}

void graphics_draw(Graphics *g, const uint8_t *gfx) {
    SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 255);
    SDL_RenderClear(g->renderer);

    SDL_SetRenderDrawColor(g->renderer, 255, 255, 255, 255);
    for (int y = 0; y < GFX_HEIGHT; y++) {
        for (int x = 0; x < GFX_WIDTH; x++) {
            if (gfx[y * GFX_WIDTH + x]) {
                SDL_Rect rect = {
                    x * GFX_SCALE,
                    y * GFX_SCALE,
                    GFX_SCALE,
                    GFX_SCALE
                };
                SDL_RenderFillRect(g->renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(g->renderer);
}

void graphics_destroy(Graphics *g) {
    if (g->renderer)
        SDL_DestroyRenderer(g->renderer);
    if (g->window)
        SDL_DestroyWindow(g->window);
    SDL_Quit();
}
