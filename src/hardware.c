#include "hardware.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Sets the 10 LEDs on the board.
 * @param led_mask A 10-bit mask to light up LEDs.
 */
void set_leds(int led_mask){
    *pLEDS = led_mask;
}

/**
 * @brief Reads the state of the 4 push buttons.
 * @return A 4-bit value representing the buttons (0 = pressed).
 */
int get_btn(void){
    // Note: Buttons are active-low (0 when pressed)
    return (*pPUSH_BUTTONS) & 0xF;    
}
    
/**
 * @brief Reads the state of the 10 toggle switches.
 * @return A 10-bit value representing the switches.
 */
int get_sw(void){
    return (*pSWITCHES) & 0x3FF; 
}

/**
 * @brief Sets one of the six 7-segment displays.
 * @param display_number The display to write to (0-5).
 * @param value The digit to display (0-9).
 */
void set_display( int display_number, int value){
    // Look up table for 7-segment display (common anode)
    static const int sev_seg_map[] = {       
        0x40, // 0
        0x79, // 1
        0x24, // 2
        0x30, // 3
        0x19, // 4
        0x12, // 5
        0x02, // 6
        0x78, // 7
        0x00, // 8
        0x10, // 9
    };
     
    int bit_pattern;

    if(value >= 0 && value <= 9){
        bit_pattern = sev_seg_map[value];
    } else {
        // Turn off all segments for invalid numbers
        bit_pattern = 0x7F;             
    }

    // Calculate the address for the specified display 
    unsigned int display_address = SEV_SEG_DISPLAY_BASE_ADDR + (display_number * 0x10);

    volatile int *display_pointer = (volatile int *) display_address;
    *display_pointer = bit_pattern; 
}