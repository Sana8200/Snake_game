/**
 * @file main.c
 * @brief Main entry point for the 2D VGA Snake game.
 *
 * Initializes hardware and runs the main game loop.
 * Manages game speed based on SW8.
 */

#include "hardware.h"   // For set_display, set_leds
#include "timer.h"      // For game tick
#include "vga_driver.h" // For VGA drawing functions
#include <stdbool.h>
#include "gameFunctions.h" // For all game logic

// Keep track of the timer speed to avoid re-initializing every loop
static int current_speed_hz = -1; 

/**
 * @brief Interrupt service routine stub.
 * (Called by boot.S but not used in this polling-based game)
 */
void handle_interrupt(unsigned cause) {
    // This is a stub; game logic is polling-based.
}

/**
 * @brief Main function: Program entry point
 */
int main() {
    // Set up the initial game state (this will also draw the first screen)
    reset_game();

    // Main game loop
    while (1) {
        // 1. Read inputs (handles pause, reset, direction)
        update_inputs();

        // 2. Check and set game speed from SW8
        int sw_val = get_sw();
        int target_speed = (sw_val & SPEED_SWITCH_BIT) ? GAME_TICK_HZ_FAST : GAME_TICK_HZ_SLOW;

        if (target_speed != current_speed_hz) {
            timer_init(target_speed);
            current_speed_hz = target_speed;
        }

        // 3. Update game logic (only happens on a timer tick)
        if (timer_check_tick()) {
            update_game_state(); // This function now handles its own drawing
        }

        // 4. Render the full display (only if needs_render is flagged)
        // This is now only used for the "Game Over" screen.
        if (needs_render) {
            render_display();
            needs_render = false;
        }
    }

    return 0; // This line should never be reached
}