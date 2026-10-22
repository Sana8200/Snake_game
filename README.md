# Snake on the DTEK-V

Snake written in bare-metal C for the DTEK-V, a RISC-V (RV32IM) processor that runs on a Terasic DE10-Lite FPGA board. The game draws straight into a memory-mapped VGA frame buffer, is played with the board's toggle switches, and shows the score on the 7-segment displays. There's no operating system and no C standard library: drawing, text, number formatting and timing are all done in this repo.

## Features

- 320×240 VGA output in 8-bit color (RGB 3-3-2)
- Pixel-drawn sprites: a snake head whose eyes face the way it's moving, a scaled body, and an apple
- A 5×7 bitmap font for the score, controls and messages
- Score on screen and on four 7-segment displays; snake length on LEDs 0–9
- Pause, reset, wrap-around or deadly walls, and two speeds
- Best score since power-on, shown on the game over screen

## Controls

| Switch | Does |
|---|---|
| SW0 | Pauses the game while it's on |
| SW1 / SW2 / SW3 / SW4 | Turn up / down / left / right |
| SW7 | Walls: off = wrap around, on = deadly |
| SW8 | Speed: off = normal, on = fast |
| SW9 | Reset: flip it on, and the new game starts when you flip it back off |

Flipping a direction switch either way turns the snake, so it doesn't matter which switches are left on. The snake can't turn back into itself.

## How it plays

The board is a 20×13 grid of 16×16-pixel cells under a 20-pixel score bar. The snake starts in the middle with five segments, heading right. Each apple adds a point and a segment. The game ends when the snake runs into itself, or into a wall when SW7 is on.

The snake moves about every 0.36 s, or every 0.14 s with SW8 on. Between steps the game reads the switches every 9 ms or so, so turning and pausing respond right away.

## Project layout

| File | What it does |
|---|---|
| `src/main.c` | Game loop: reset, one snake step, then wait for the next step while reading the switches |
| `src/snake.c` | Game logic: movement, collisions, food, score, pause and game over messages |
| `src/vga_driver.c` | Pixels, lines, rectangles, text and sprites in the VGA frame buffer |
| `src/hardware.c` | LEDs, switches and 7-segment displays |
| `src/delay_asm.S` | Cycle-counted busy-wait used for timing |
| `src/boot.S`, `src/dtekv-lib.c`, `src/dtekv-script.lds`, `src/softfloat.a` | Startup code, exception handler, linker script and soft-float library from the DTEK-V framework |

## Memory map

| Device | Address |
|---|---|
| VGA frame buffer (320×240, one byte per pixel) | `0x08000000` |
| LEDs | `0x04000000` |
| Switches | `0x04000010` |
| 7-segment displays (one every `0x10`) | `0x04000050` |

## Building

You need a RISC-V GNU toolchain. The Makefile expects the `riscv32-unknown-elf-` prefix; set `TOOLCHAIN` to use a different one.

```bash
cd src
make
```

This builds `main.elf`, the raw image `main.bin`, and a disassembly in `main.elf.txt`.

`make run` passes `main.bin` to the DTEK-V upload tools, which aren't part of this repo. Point `TOOL_DIR` at your copy:

```bash
make run TOOL_DIR=/path/to/dtekv-tools
```

To play, you need a DE10-Lite with the DTEK-V processor loaded and a VGA monitor.

## Limitations and ideas

- Timing is a busy-wait, so each step also includes the time it takes to draw. Driving the loop from the board's interval timer would make it exact.
- The best score is kept only until the board is powered off.
- Ideas: speed up as the snake grows, show the best score on the spare 7-segment displays.

## Credits and license

The startup code, exception handler, linker script and soft-float library come from the DTEK-V framework by Artur Podobas, Wiktor Szczerek and Pedro Antunes. The game is by Sana Monhaseri.

Released under the BSD-style license in [COPYING](COPYING).
