#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "vga_driver.h"
#include "hardware.h"

// --- Config ---
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)

// --- Switch Controls (10 toggle switches on DE10-Lite) ---
#define PAUSE_SWITCH_BIT     (1 << 0) // SW0 - Pause while ON
#define UP_SWITCH_BIT        (1 << 1) // SW1 - Move Up
#define DOWN_SWITCH_BIT      (1 << 2) // SW2 - Move Down
#define LEFT_SWITCH_BIT      (1 << 3) // SW3 - Move Left
#define RIGHT_SWITCH_BIT     (1 << 4) // SW4 - Move Right
#define WALL_MODE_SWITCH_BIT (1 << 7) // SW7 - Wall mode (0=wrap, 1=deadly)
#define SPEED_SWITCH_BIT     (1 << 8) // SW8 - Speed (0=normal, 1=fast)
#define RESET_SWITCH_BIT     (1 << 9) // SW9 - Reset game

#define DIRECTION_SWITCH_MASK (UP_SWITCH_BIT | DOWN_SWITCH_BIT | LEFT_SWITCH_BIT | RIGHT_SWITCH_BIT)

// --- Functions ---
void snake_init(void);
void snake_update_input(void);
void snake_update_state(void);

#endif
