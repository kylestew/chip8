#include "chip8.h"
#include "graphics.h"
#include "rom.h"

#include <SDL.h>
#include <stdio.h>

#define CPU_HZ 500
#define TIMER_HZ 60
#define CYCLES_PER_FRAME (CPU_HZ / TIMER_HZ)
#define FRAME_MS (1000 / TIMER_HZ)

// Map SDL keycode to CHIP-8 key index (0x0-0xF), or -1 if not a CHIP-8 key
static int key_from_sdl(SDL_Keycode sym) {
    switch (sym) {
    case SDLK_1: return 0x1;
    case SDLK_2: return 0x2;
    case SDLK_3: return 0x3;
    case SDLK_4: return 0xC;
    case SDLK_q: return 0x4;
    case SDLK_w: return 0x5;
    case SDLK_e: return 0x6;
    case SDLK_r: return 0xD;
    case SDLK_a: return 0x7;
    case SDLK_s: return 0x8;
    case SDLK_d: return 0x9;
    case SDLK_f: return 0xE;
    case SDLK_z: return 0xA;
    case SDLK_x: return 0x0;
    case SDLK_c: return 0xB;
    case SDLK_v: return 0xF;
    default: return -1;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    // Init chip8 core
    Chip8 chip8;
    chip8_init(&chip8);

    // Load ROM
    if (rom_load(chip8.memory + CHIP8_ROM_START, sizeof(chip8.memory) - CHIP8_ROM_START, argv[1]) != 0)
        return 1;

    // Init graphics (SDL)
    Graphics gfx;
    if (graphics_init(&gfx) != 0)
        return 1;

    // Emulation loop
    int running = 1;
    while (running) {
        uint32_t frame_start = SDL_GetTicks();

        // 1. Input
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
            if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                int idx = key_from_sdl(e.key.keysym.sym);
                if (idx >= 0)
                    chip8.key[idx] = (e.type == SDL_KEYDOWN) ? 1 : 0;
            }
        }

        // 2. CPU -- run N cycles per frame
        for (int i = 0; i < CYCLES_PER_FRAME; i++)
            chip8_cycle(&chip8);

        // 3. Timers -- tick at 60 Hz
        chip8_tick_timers(&chip8);

        // 4. Render -- only when draw_flag is set
        if (chip8.draw_flag) {
            graphics_draw(&gfx, chip8.gfx);
            chip8.draw_flag = 0;
        }

        // 5. Frame cap
        uint32_t elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < FRAME_MS)
            SDL_Delay(FRAME_MS - elapsed);
    }

    graphics_destroy(&gfx);
    return 0;
}
