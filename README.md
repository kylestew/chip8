# CHIP-8 Interpreter

A CHIP-8 emulator/interpreter written in C using SDL2 for graphics.

## Dependencies

- C compiler (cc/gcc/clang)
- SDL2 (`brew install sdl2` on macOS)

## Build

```
make
```

## Run

```
./chip8 <path-to-rom>
```

Example:

```
./chip8 roms/test_opcode.ch8
```

## Project Structure

| File | Purpose |
|------|---------|
| `chip8.h` / `chip8.c` | Emulator core -- state, init, cycle, timers |
| `rom.h` / `rom.c` | ROM file loader |
| `graphics.h` / `graphics.c` | SDL2 window and rendering |
| `main.c` | Entry point, emulation loop, input |

## Controls

- **1 2 3 4** / **Q W E R** / **A S D F** / **Z X C V** — CHIP-8 keypad (hex 1-4, 4-6, 7-9, A-0-B-F)
- **ESC** — exit

## Status

Full CHIP-8 instruction set implemented:

- Emulation loop with 60 Hz frame cap and configurable CPU clock (500 Hz)
- ROM loader with debug hex dump (`-DDEBUG`)
- SDL2 graphics renderer (64x32 scaled 10x)
- Font set loaded into memory at 0x050
- Delay and sound timers (60 Hz countdown)
- Keyboard input mapping
- Opcode interpreter:
  - `00E0` / `00EE` — clear screen / return from subroutine
  - `1NNN` / `2NNN` — jump / call subroutine
  - `3XNN` / `4XNN` / `5XY0` / `9XY0` — skip conditionals (== NN, != NN, == VY, != VY)
  - `6XNN` / `7XNN` — set VX / add to VX
  - `8XY0`–`8XYE` — ALU (set, OR, AND, XOR, add, sub, shifts)
  - `ANNN` / `BNNN` / `CXNN` — set I / jump V0+NNN / random
  - `DXYN` — draw sprite
  - `EX9E` / `EXA1` — skip if key pressed / not pressed
  - `FX07` / `FX0A` / `FX15` / `FX18` — timers, wait key
  - `FX1E` / `FX29` / `FX33` / `FX55` / `FX65` — I ops, font, BCD, store/load regs

Sound timer runs but audio output is not implemented.
