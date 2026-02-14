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

## Status

Work in progress. Functional so far:

- Emulation loop with 60 Hz frame cap and configurable CPU clock
- ROM loader with debug hex dump (`-DDEBUG`)
- SDL2 graphics renderer (64x32 scaled 10x)
- Font set loaded into memory at init
- Opcode interpreter (partial):
  - `00E0` -- clear screen
  - `0NNN` -- ignored (machine language call)
  - `1NNN` -- jump
  - `2NNN` -- call subroutine
  - `3XNN` -- skip if VX == NN
  - `4XNN` -- skip if VX != NN
  - `5XY0` -- skip if VX == VY
  - `6XNN` -- set VX = NN
  - `7XNN` -- add NN to VX
  - `9XY0` -- skip if VX != VY
  - `ANNN` -- set I = NNN
  - `DXYN` -- draw sprite

Not yet implemented:

- `00EE` -- return from subroutine
- `8XY_` -- ALU operations
- `BNNN` -- jump with offset
- `CXNN` -- random
- `EX__` -- key input instructions
- `FX__` -- timers, BCD, memory load/store
- Timer decrement logic
- Keyboard input mapping
