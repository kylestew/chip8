#include "chip8.h"
#include "graphics.h"
#include "rom.h"

#include <SDL.h>
#include <stdio.h>

#define CPU_HZ 500
#define TIMER_HZ 60
#define CYCLES_PER_FRAME (CPU_HZ / TIMER_HZ)
#define FRAME_MS (1000 / TIMER_HZ)

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
            // TODO: map SDL keycodes to chip8.key[0x0..0xF]
        }

        // 2. CPU -- run N cycles per frame
        for (int i = 0; i < CYCLES_PER_FRAME; i++)
            chip8_cycle(&chip8);

        // 3. Timers -- tick at 60 Hz
        // chip8_tick_timers(&chip8);

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
