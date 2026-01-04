#include "hardware.h"
#include "vga_driver.h"
#include "snake.h" 
#include "delay.h"
#include "dtekv-lib.h"

void handle_interrupt(unsigned cause) {
    // Stub for interrupt handling (if needed later)
}

int main() {
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
            while(get_sw() & RESET_SWITCH_BIT) {
                delay(50);
            }
        }
        
        // Update game input (buttons and switches)
        snake_update_input();
        
        // Update game state (move snake, check collisions)
        snake_update_state();

        // Speed control using SW8
        if (get_sw() & SPEED_SWITCH_BIT) {
            delay(150);  // Fast mode
        } else {
            delay(400);  // Normal mode
        }
    }
    
    return 0;
}