#ifndef GAMEFUNCTIONS_H
#define GAMEFUNCTIONS_H

#include "vga_driver.h" // For game grid/screen definitions
#include <stdbool.h>

// --- Game Constants ---
#define GAME_TICK_HZ_SLOW 8   // How many times the snake moves per second (Slow)
#define GAME_TICK_HZ_FAST 15  // How many times the snake moves per second (Fast)

// --- Control Definitions ---
// Keys (Active-Low)
#define KEY_UP_BIT    (1 << 0)
#define KEY_DOWN_BIT  (1 << 1)
#define KEY_LEFT_BIT  (1 << 2)
#define KEY_RIGHT_BIT (1 << 3)
// Switches (Active-High)
#define PAUSE_SWITCH_BIT     (1 << 0)
#define WALL_MODE_SWITCH_BIT (1 << 7)
#define SPEED_SWITCH_BIT     (1 << 8)
#define RESET_SWITCH_BIT     (1 << 9)


// --- Game State Variables (Declared as 'extern') ---
// These are *defined* in gameFunctions.c
extern int snake_body_x[MAX_SNAKE_LENGTH];
extern int snake_body_y[MAX_SNAKE_LENGTH];
extern int snake_length;

extern int food_x;
extern int food_y;

extern char direction; // Current direction
extern char next_direction; // Direction from input

extern int score;
extern bool game_over;
extern bool needs_render; // Flag to redraw the screen
extern bool is_paused;


// --- Function Prototypes ---
void score_to_string(int s, char* buf);
void place_new_food();
void reset_game();
void update_inputs();
void update_game_state();
void render_display();
void update_seven_segment(int score);

#endif // GAMEFUNCTIONS_H