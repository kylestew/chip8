#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include <stdint.h>

#define GFX_SCALE    10
#define GFX_WIDTH    64
#define GFX_HEIGHT   32

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
} Graphics;

// Create SDL window + renderer. Returns 0 on success, -1 on error.
int graphics_init(Graphics *g);

// Draw a 64x32 pixel buffer to the window.
void graphics_draw(Graphics *g, const uint8_t *gfx);

// Destroy window, renderer, and call SDL_Quit.
void graphics_destroy(Graphics *g);

#endif
