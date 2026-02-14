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
            c->pc = pop(c);
            break;

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

    case 8: // 8XYN - ALU operations
    {
        uint8_t x = X(opcode), y = Y(opcode);
        uint16_t sum;
        switch (N(opcode)) {
        case 0x0: // 8XY0: VX = VY
            c->V[x] = c->V[y];
            break;
        case 0x1: // 8XY1: VX |= VY
            c->V[x] |= c->V[y];
            break;
        case 0x2: // 8XY2: VX &= VY
            c->V[x] &= c->V[y];
            break;
        case 0x3: // 8XY3: VX ^= VY
            c->V[x] ^= c->V[y];
            break;
        case 0x4: // 8XY4: VX += VY, VF = carry
            sum = (uint16_t)c->V[x] + c->V[y];
            c->V[0xF] = (sum > 255) ? 1 : 0;
            c->V[x] = (uint8_t)sum;
            break;
        case 0x5: // 8XY5: VX -= VY, VF = NOT borrow
            c->V[0xF] = (c->V[x] >= c->V[y]) ? 1 : 0;
            c->V[x] -= c->V[y];
            break;
        case 0x6: // 8XY6: VF = VX & 1, VX >>= 1
            c->V[0xF] = c->V[x] & 1;
            c->V[x] >>= 1;
            break;
        case 0x7: // 8XY7: VX = VY - VX, VF = NOT borrow
            c->V[0xF] = (c->V[y] >= c->V[x]) ? 1 : 0;
            c->V[x] = c->V[y] - c->V[x];
            break;
        case 0xE: // 8XYE: VF = VX >> 7, VX <<= 1
            c->V[0xF] = (c->V[x] >> 7) & 1;
            c->V[x] <<= 1;
            break;
        default:
            printf("ILLEGAL OPCODE: %04hx at %03hx\n", opcode, c->pc);
            exit(1);
        }
    } break;

    case 9: // 9XY0 - Skip the following instruction if the value of register VX is not equal to the value of register
            // VY
    if (c->V[X(opcode)] != c->V[Y(opcode)])
        c->pc += 2; // one instruction
    break;

case 0xA: // ANNN: Store memory address NNN in register I
    c->I = NNN(opcode);
    break;

case 0xB: // BNNN: Jump to NNN + V0
    c->pc = NNN(opcode) + c->V[0];
    break;

case 0xC: // CXNN: VX = random byte AND NN
    c->V[X(opcode)] = (rand() & 0xFF) & NN(opcode);
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

case 0xE: // EX9E / EXA1 - Skip if key pressed / not pressed
    switch (NN(opcode)) {
    case 0x9E: // SKP Vx
        if (c->key[c->V[X(opcode)] & 0x0F])
            c->pc += 2;
        break;
    case 0xA1: // SKNP Vx
        if (!(c->key[c->V[X(opcode)] & 0x0F]))
            c->pc += 2;
        break;
    default:
        printf("ILLEGAL OPCODE: %04hx at %03hx\n", opcode, c->pc);
        exit(1);
    }
    break;

case 0xF: // FX__ - Timers, input, memory, etc.
    switch (NN(opcode)) {
    case 0x07: // FX07: Vx = delay_timer
        c->V[X(opcode)] = c->delay_timer;
        break;
    case 0x0A: // FX0A: Wait for key press, store in Vx
    {
        int found = 0;
        for (int k = 0; k < CHIP8_NUM_KEYS; k++) {
            if (c->key[k]) {
                c->V[X(opcode)] = (uint8_t)k;
                found = 1;
                break;
            }
        }
        if (!found)
            c->pc -= 2; // re-execute next cycle
        break;
    }
    case 0x15: // FX15: delay_timer = Vx
        c->delay_timer = c->V[X(opcode)];
        break;
    case 0x18: // FX18: sound_timer = Vx
        c->sound_timer = c->V[X(opcode)];
        break;
    case 0x1E: // FX1E: I += Vx
        c->I += c->V[X(opcode)];
        break;
    case 0x29: // FX29: I = font sprite address for digit Vx
        c->I = CHIP8_FONT_START + (c->V[X(opcode)] & 0x0F) * 5;
        break;
    case 0x33: // FX33: BCD - store hundreds, tens, ones at I, I+1, I+2
    {
        uint8_t val = c->V[X(opcode)];
        c->memory[c->I]     = val / 100;
        c->memory[c->I + 1] = (val / 10) % 10;
        c->memory[c->I + 2] = val % 10;
        break;
    }
    case 0x55: // FX55: Store V0..Vx in memory starting at I
        for (int i = 0; i <= X(opcode); i++)
            c->memory[c->I + i] = c->V[i];
        break;
    case 0x65: // FX65: Load V0..Vx from memory starting at I
        for (int i = 0; i <= X(opcode); i++)
            c->V[i] = c->memory[c->I + i];
        break;
    default:
        printf("ILLEGAL OPCODE: %04hx at %03hx\n", opcode, c->pc);
        exit(1);
    }
    break;

default:
    printf("ILLEGAL OPCODE: %04hx at %03hx\n", opcode, c->pc);
    exit(1);
    break;
}

// Execute
}

void chip8_tick_timers(Chip8 *c) {
    if (c->delay_timer > 0) c->delay_timer--;
    if (c->sound_timer > 0) c->sound_timer--;
}
