#include "hardware.h"
#include "snake.h"
#include "delay.h"

void handle_interrupt(unsigned cause) {
    // Called from boot.S; this game doesn't enable any interrupts
}

// One unit is 3000 loop iterations of 9 cycles, about 0.9 ms at 30 MHz
#define DELAY_UNIT_CYCLES 27000u

// Time between snake steps, and how often the switches are read meanwhile
#define NORMAL_STEP_UNITS 400
#define FAST_STEP_UNITS   150
#define POLL_UNITS        10

static void game_delay(uint32_t units) {
    delay_cycles_asm(units * DELAY_UNIT_CYCLES);
}

// Waits for the next step, reading the switches every few ms so that
// pausing and turning respond right away
static void wait_for_next_step(void) {
    uint32_t units = (get_sw() & SPEED_SWITCH_BIT) ? FAST_STEP_UNITS : NORMAL_STEP_UNITS;

    for (uint32_t waited = 0; waited < units; waited += POLL_UNITS) {
        snake_update_input();
        game_delay(POLL_UNITS);
    }
}

int main(void) {
    // Turn off all LEDs at start
    set_leds(0);

    // Initialize the game
    snake_init();

    // Main game loop
    while (1) {
        // Check for reset switch (SW9)
        if (get_sw() & RESET_SWITCH_BIT) {
            snake_init();
            // Wait for switch to be released to avoid multiple resets
            while (get_sw() & RESET_SWITCH_BIT) {
                game_delay(50);
            }
        }

        // Update game state (move snake, check collisions)
        snake_update_state();

        // Wait for the next step (SW8 picks the speed)
        wait_for_next_step();
    }

    return 0;
}
