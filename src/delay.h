#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

// Busy-waits for about the given number of clock cycles (delay_asm.S)
void delay_cycles_asm(uint32_t cycles);

#endif  /* DELAY_H */
