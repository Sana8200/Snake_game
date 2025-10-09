// file: src/main.c (Final Integrated Version)

#include "graphics.h"
#include "input.h"
#include "game.h"
#include "peripherals.h"


// Button definitions (You may need to debug and change these later)
#define BTN_RIGHT (1 << 0)
#define BTN_LEFT  (1 << 1)
#define BTN_DOWN  (1 << 2)
#define BTN_UP    (1 << 3)
#define BTN_ANY   (BTN_RIGHT | BTN_LEFT | BTN_DOWN | BTN_UP)

// LED status definitions
#define LED_PLAYING   1
#define LED_GAME_OVER 2

// Switch definitions
#define SWITCH_FAST_MODE 1 // Use switch 0 for speed control

void render_game(const GameState* state) {
    clear_screen();
    draw_rect(state->apple.x, state->apple.y, 1, 1, COLOR_RED);
    for (int i = 0; i < state->snake.length; ++i) {
        unsigned short color = (i == 0) ? COLOR_YELLOW : COLOR_GREEN;
        draw_rect(state->snake.body[i].x, state->snake.body[i].y, 1, 1, color);
    }
    draw_score(5, 5, state->score, COLOR_WHITE);
}

int main() {
    // ... Initialization code ...
    // vga_clear_screen(...);

    // The phase_offset variable is declared and initialized to 0 ONCE.
    volatile int phase_offset = 0;  // <-- CORRECT: Must be OUTSIDE the loop.

    while (1) {
        // int phase_offset = 0; // <-- WRONG: If it's declared INSIDE, the wave will not move.

        // ...
        // The code to clear the screen and draw the wave...
        // ...

        // This is the line that actually makes the wave move on the next frame.
        phase_offset++;

        // The delay to control speed
        delay(30000);
    }

    return 0;
}

void handle_interrupt(void) {
    // Required by boot.S but not used by our game.
}