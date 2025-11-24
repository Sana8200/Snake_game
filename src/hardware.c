#include "hardware.h"
#include <stdint.h>
#include <stdbool.h>


void set_leds(int led_mask){
    *pLEDS = led_mask;
}


int get_btn(void){
    // Note: Buttons are active-low (0 when pressed)
    return (*pPUSH_BUTTONS) & 0xF;    
}
    

int get_sw(void){
    return (*pSWITCHES) & 0x3FF; 
}

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