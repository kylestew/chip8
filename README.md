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

Work in progress. The emulation loop, ROM loader, and graphics renderer are
functional. The `chip8_cycle` opcode interpreter is not yet implemented.
