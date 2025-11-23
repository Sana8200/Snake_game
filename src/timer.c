#include "timer.h"

/**
 * @brief Initializes the hardware timer to tick at a specific frequency.
 */
void timer_init(int target_frequency_hz) {
    // 1. Stop the timer
    *TIMER_CTRL = TIMER_CTRL_STOP; 
    
    // 2. Clear the Time-Out (TO) status bit
    *TIMER_STATUS = 0;

    // 3. Calculate the period count
    // Formula: Clock_Freq / Target_Freq
    int period_count = SYSTEM_CLOCK_FREQ / target_frequency_hz;

    // 4. Write the period to the Low and High registers
    *TIMER_PERIODL = (period_count & 0xFFFF);        // Lower 16 bits
    *TIMER_PERIODH = ((period_count >> 16) & 0xFFFF); // Upper 16 bits

    // 5. Start the timer in continuous mode
    *TIMER_CTRL = TIMER_CTRL_START | TIMER_CTRL_CONT;
}

/**
 * @brief Checks if the timer has ticked since the last check (for polling).
 */
bool timer_check_tick() {
    // Check bit 0 (TO - Timeout) of the status register
    if (*TIMER_STATUS & TIMER_STATUS_TO) {
        // Clear the status bit by writing to it
        *TIMER_STATUS = 0; 
        return true;
    }
    return false;
}