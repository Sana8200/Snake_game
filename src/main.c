#include "hardware.h"
#include "vga_driver.h"
#include "snake.h" 
#include "lib.h"

void handle_interrupt(unsigned cause) {
    // This is a stub; game logic is polling-based.
}


#include "hardware.h"
#include "snake.h"

int main() {
    set_leds(0); 
    snake_init();


    while (1) {

        if (get_sw() & RESET_SWITCH_BIT) {
            snake_init();
            while(get_sw() & RESET_SWITCH_BIT);
        }

        snake_update_input();
        snake_update_state();

        if (get_sw() & SPEED_SWITCH_BIT) {
            delay(150); 
        } else {
            delay(4000); 
        }
    }
    
    // Should never reach here
    return 0;
}