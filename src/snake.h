#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include <stdlib.h>
#include "vga_driver.h"
#include "hardware.h"

// --- Config ---
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)

// --- Controls ---
// Assuming SW0-9. Adjust bits if your board matches different switches.
#define PAUSE_SWITCH_BIT     (1 << 0) // SW0
#define WALL_MODE_SWITCH_BIT (1 << 7) // SW7
#define SPEED_SWITCH_BIT     (1 << 8) // SW8
#define RESET_SWITCH_BIT     (1 << 9) // SW9

// Buttons (Active Low logic is handled in snake.c)
#define KEY_UP_BIT    (1 << 0) // KEY0
#define KEY_DOWN_BIT  (1 << 1) // KEY1
#define KEY_LEFT_BIT  (1 << 2) // KEY2
#define KEY_RIGHT_BIT (1 << 3) // KEY3

// --- Public Global State ---
extern int body_x[MAX_SNAKE_LENGTH];
extern int body_y[MAX_SNAKE_LENGTH];
extern int length;
extern int score;
extern bool game_over;
extern bool is_paused;

// --- Functions ---
void snake_init();
void snake_update_input();
void snake_update_state();
int rand_range(int min, int max);

#endif