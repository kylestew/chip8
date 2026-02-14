#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_RAM_SIZE 4096
#define CHIP8_STACK_SIZE 16
#define CHIP8_NUM_REGS 16
#define CHIP8_NUM_KEYS 16
#define CHIP8_DISPLAY_W 64
#define CHIP8_DISPLAY_H 32
#define CHIP8_ROM_START 0x200
#define CHIP8_FONT_START 0x050

typedef struct {
    // 4K system memory
    /*== Memory Map ==
      0x000-0x1FF - interpreter
      0x050-0x0A0 - built in 4x5 pixel font set (0-F)
      0x200-0xFFF - program ROM and working RAM
      -================*/
    uint8_t memory[CHIP8_RAM_SIZE];

    // 15 8-bit general purpose registers V0-VE
    // VF is used for 'carry flag'
    uint8_t V[CHIP8_NUM_REGS];

    // index register (I)
    uint16_t I;
    // program counter (PC)
    uint16_t pc;

    // return stack
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t sp;

    // timer registers - count down at 60Hz
    uint8_t delay_timer;
    uint8_t sound_timer; // buzzer sounds whenever > 0

    // B/W - 64x32 (2048 pixels)
    uint8_t gfx[CHIP8_DISPLAY_W * CHIP8_DISPLAY_H];

    // hex keypad input (0x0-0xF)
    uint8_t key[CHIP8_NUM_KEYS];

    // set by DXYN opcode, cleared after render
    uint8_t draw_flag;
} Chip8;

// Zero all state, load font into memory, set pc = ROM_START
void chip8_init(Chip8 *c);

// Execute one fetch-decode-execute cycle
void chip8_cycle(Chip8 *c);

// Decrement delay_timer and sound_timer (call once per 60 Hz tick)
void chip8_tick_timers(Chip8 *c);

#endif
