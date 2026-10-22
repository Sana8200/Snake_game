# delay_asm.S
# Busy-wait used for the game's frame timing.

    .text
    .globl delay_cycles_asm

# void delay_cycles_asm(uint32_t cycles)
# Arguments: a0 = number of cycles to delay
delay_cycles_asm:
    li t0, 9                    # Load divisor (9 cycles per iteration)
    divu t1, a0, t0             # t1 = cycles / 9 (iterations needed)
    beqz t1, exit_this_delay    # If zero iterations, exit immediately

delay_loop:
    addi t1, t1, -1             # Decrement iteration counter
    bnez t1, delay_loop         # Loop if not zero

exit_this_delay:
    ret                         # Return to caller

    .size delay_cycles_asm, .-delay_cycles_asm