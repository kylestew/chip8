#include "chip8.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NNN(op) ((op) & 0x0FFF)
#define NN(op) ((op) & 0x00FF)
#define X(op) (((op) >> 8) & 0x0F)
#define Y(op) (((op) >> 4) & 0x0F)
#define N(op) ((op) & 0x000F)

void push(Chip8 *c, uint16_t v) { c->stack[c->sp++] = v; }
uint16_t pop(Chip8 *c) { return c->stack[--c->sp]; }

// FONT - 80 bytes, 5 bytes per hex digit (0-F)
static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8 *c) {
    memset(c, 0, sizeof(*c));
    c->pc = CHIP8_ROM_START;

    memcpy(c->memory + CHIP8_FONT_START, fontset, sizeof(fontset));
}

void chip8_cycle(Chip8 *c) {
    // Fetch (stored big-endian)
    uint16_t opcode = c->memory[c->pc++] << 8;
    opcode |= c->memory[c->pc++];

    // Decode
    // opcodes are roughly grouped by the most sig nibble
    uint8_t nibble = (opcode & 0xF000) >> 12;
    switch (nibble) {
    case 0:
        switch (opcode) {
        case 0x00E0: // clear screen
            memset(c->gfx, 0, sizeof(c->gfx));
            c->draw_flag = 1;
            break;

        case 0x00EE: // Return from Subroutine
            printf("RTS: %04hx\n", opcode);
            exit(1);

        default: // 0NNN - ignored (machine language call, not used in modern interpreters)
            break;
        }
        break;

    case 1: // Jump to address NNN
        c->pc = NNN(opcode);
        break;

    case 2: // Subroutine call NNN
        push(c, c->pc);
        c->pc = NNN(opcode);
        break;

    case 3: // 3XNN - Skip the following instruction if the value of register VX equals NN
        if (c->V[X(opcode)] == NN(opcode))
            c->pc += 2; // one instruction
        break;

    case 4: // 4XNN - Skip the following instruction if the value of register VX is not equal to NN
        if (c->V[X(opcode)] != NN(opcode))
            c->pc += 2; // one instruction

        break;

    case 5: // 5XY0 - Skip the following instruction if the value of register VX is equal to the value of register VY
        if (c->V[X(opcode)] == c->V[Y(opcode)])
            c->pc += 2; // one instruction
        break;

    case 6: // 6XNN: Store number NN in register VX
        c->V[X(opcode)] = NN(opcode);
        break;

    case 7: // 7XNN: Add the value NN to register VX
        c->V[X(opcode)] += NN(opcode);
        break;

        //..

    case 9: // 9XY0 - Skip the following instruction if the value of register VX is not equal to the value of register
            // VY
        if (c->V[X(opcode)] != c->V[Y(opcode)])
            c->pc += 2; // one instruction
        break;

    case 0xA: // ANNN: Store memory address NNN in register I
        c->I = NNN(opcode);
        break;

    case 0xB:
        break;

    case 0xD: // DXYN - Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored
              // in I Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
    {
        uint8_t x = c->V[X(opcode)];
        uint8_t y = c->V[Y(opcode)];
        uint8_t n = N(opcode);
        c->V[0xF] = 0;

        for (int row = 0; row < n; row++) {
            uint8_t sprite_byte = c->memory[c->I + row];
            for (int col = 0; col < 8; col++) {
                if (sprite_byte & (0x80 >> col)) { // if bit set in sprite
                    int px  = (x + col) % CHIP8_DISPLAY_W;
                    int py  = (y + row) % CHIP8_DISPLAY_H;
                    int idx = py * CHIP8_DISPLAY_W + px;
                    if (c->gfx[idx])
                        c->V[0xF] = 1; // collision: pixel was on, now turns off
                    c->gfx[idx] ^= 1;  // xor composite
                }
            }
        }
        c->draw_flag = 1;
    } break;

    default:
        printf("ILLEGAL OPCODE: %04hx at %03hx\n", opcode, c->pc);
        exit(1);
        break;
    }

    // Execute
}

void chip8_tick_timers(Chip8 *c) {
    // TODO: decrement delay_timer and sound_timer if > 0
    (void) c;
}
