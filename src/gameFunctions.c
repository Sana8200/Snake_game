#include "gameFunctions.h"
#include "hardware.h" // For get_sw(), get_btn(), set_leds()

// --- Predefined Apple Locations (Adapted from your new code for 20x13 grid) ---
static const int apple_xpos[] = { 10, 5, 15, 2, 18, 8, 12, 1, 19, 7, 14, 3 };
static const int apple_ypos[] = { 5, 2, 10, 1, 12, 8, 3, 6, 9, 11, 4, 7 };
static const int num_apples = 12; // Total number of predefined apples

// --- Game State Variable Definitions ---
// Actual storage for the global variables declared in the .h file
int snake_body_x[MAX_SNAKE_LENGTH];
int snake_body_y[MAX_SNAKE_LENGTH];
int snake_length;

int food_x;
int food_y;

char direction; 
char next_direction;

int score;
bool game_over;
bool needs_render; 
bool is_paused = false;

// To detect a single switch toggle
static bool last_pause_sw_state = false; 

/**
 * @brief Updates the 4 right-most 7-segment displays with the score.
 */
void update_seven_segment(int score) {
    set_display(0, score % 10);
    set_display(1, (score / 10) % 10);
    set_display(2, (score / 100) % 10);
    set_display(3, (score / 1000) % 10);
    // Turn off the other two
    set_display(4, -1); 
    set_display(5, -1);
}

/**
 * @brief Converts a simple integer score to a string.
 */
void score_to_string(int s, char* buf) {
    if (s == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    int i = 0;
    int temp = s;
    while (temp > 0) {
        buf[i++] = (temp % 10) + '0';
        temp /= 10;
    }

    // Reverse the string
    buf[i] = '\0';
    for (int j = 0; j < i / 2; j++) {
        char t = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = t;
    }
}

/**
 * @brief Places a new piece of food on the board from the predefined list.
 */
void place_new_food() {
    // Get the next apple from the list, looping back if necessary
    food_x = apple_xpos[score % num_apples];
    food_y = apple_ypos[score % num_apples];
    
    // Check if the new food spot is on the snake
    // If so, just skip this apple and wait for the next
    for (int i = 0; i < snake_length; i++) {
        if (food_x == snake_body_x[i] && food_y == snake_body_y[i]) {
            // Food spawned on snake! Increment score to "eat" it immediately.
            // This will advance to the next apple position on the next tick.
            score++; 
            return;
        }
    }
    // Note: Food is drawn in update_game_state() or render_display()
}

/**
 * @brief Resets the game to its initial state. (Adapted from your snake_init)
 */
void reset_game() {
    // Logic from your snake_init()
    snake_length = 8;
    direction = 'r';
    next_direction = 'r';
    score = 0;

    game_over = false;
    is_paused = false;

    // Set starting position (adapted from your snake_init)
    int start_row = 6; // ~Middle of 13 rows
    int head_position = 10; // ~Middle of 20 cols
    for(int i = 0; i < snake_length; i++){
        snake_body_x[i] = head_position - i;
        snake_body_y[i] = start_row;
    }
    
    // Place the first apple (apple[0])
    place_new_food();

    // Update hardware
    update_seven_segment(score);
    // Create bitmask for LEDs, e.g., length 8 = 0b11111111
    set_leds((1 << snake_length) - 1); 

    // Force a full redraw for the new game
    needs_render = true;
    render_display(); 
    needs_render = false; // Un-flag it
}

/**
 * @brief Reads the switches and buttons to update game controls.
 */
void update_inputs() {
    int sw_val = get_sw();   // Read all switches
    int btn_val = get_btn(); // Read all buttons (active-low, 0 = pressed)

    // 1. Check for Reset (SW9) - This is a "hold" reset
    if (sw_val & RESET_SWITCH_BIT) {
        reset_game();
        return; // Reset overrides all other input
    }

    // 2. Check for Pause (SW0) - This is a "toggle"
    bool pause_sw_state = (sw_val & PAUSE_SWITCH_BIT);
    if (pause_sw_state && !last_pause_sw_state) { // Check for rising edge
        is_paused = !is_paused;
        if(is_paused) {
             vga_draw_string(140, 120, "PAUSED", COLOR_YELLOW);
        } else {
            // "Un-draw" the pause message by redrawing the full game area
            render_display();
        }
    }
    last_pause_sw_state = pause_sw_state;


    if (game_over || is_paused) return; // Don't take direction input

    // 3. Read direction from KEYs (adapted from your changedirection())
    if (!(btn_val & KEY_UP_BIT) && direction != 'd') {
        next_direction = 'u';
    }
    else if (!(btn_val & KEY_DOWN_BIT) && direction != 'u') {
        next_direction = 'd';
    }
    else if (!(btn_val & KEY_LEFT_BIT) && direction != 'r') {
        next_direction = 'l';
    }
    else if (!(btn_val & KEY_RIGHT_BIT) && direction != 'l') {
        next_direction = 'r';
    }
}


/**
 * @brief Updates the game logic by one tick.
 * This function now handles drawing to prevent flicker.
 */
void update_game_state() {
    if (game_over || is_paused) {
        return;
    }

    // Apply the buffered direction
    direction = next_direction;

    // Store old tail position to erase it later
    int old_tail_x = snake_body_x[snake_length - 1];
    int old_tail_y = snake_body_y[snake_length - 1];

    // Store old head position to draw the new "neck"
    int old_head_x = snake_body_x[0];
    int old_head_y = snake_body_y[0];

    // Move the body (from your movingsnake())
    for (int i = snake_length - 1; i > 0; i--) {
        snake_body_x[i] = snake_body_x[i - 1];
        snake_body_y[i] = snake_body_y[i - 1];
    }

    // Move the head (from your movingsnake())
    if (direction == 'r') snake_body_x[0] = old_head_x + 1;
    if (direction == 'l') snake_body_x[0] = old_head_x - 1;
    if (direction == 'u') snake_body_y[0] = old_head_y - 1; // VGA Y=0 is top
    if (direction == 'd') snake_body_y[0] = old_head_y + 1;

    // --- Check for Collisions ---
    bool ate_food = false;

    // 1. Wall collision (check SW7, based on your hit_border())
    bool walls_deadly = (get_sw() & WALL_MODE_SWITCH_BIT);

    if (snake_body_x[0] < 0) {
        if (walls_deadly) game_over = true; else snake_body_x[0] = GRID_WIDTH - 1;
    } else if (snake_body_x[0] >= GRID_WIDTH) {
        if (walls_deadly) game_over = true; else snake_body_x[0] = 0;
    } else if (snake_body_y[0] < 0) {
        if (walls_deadly) game_over = true; else snake_body_y[0] = GRID_HEIGHT - 1;
    } else if (snake_body_y[0] >= GRID_HEIGHT) {
        if (walls_deadly) game_over = true; else snake_body_y[0] = 0;
    }

    if (game_over) {
        needs_render = true; // Flag for the "Game Over" screen
        return;
    }

    // 2. Self collision (from your hit_self())
    for (int i = 1; i < snake_length; i++) {
        if (snake_body_x[0] == snake_body_x[i] && snake_body_y[0] == snake_body_y[i]) {
            game_over = true;
            needs_render = true; // Flag for the "Game Over" screen
            return;
        }
    }

    // 3. Food collision (from your found_apple() and eatingapple())
    if (snake_body_x[0] == food_x && snake_body_y[0] == food_y) {
        score++;
        ate_food = true;
        if(snake_length < MAX_SNAKE_LENGTH) {
            snake_length++;
            // Don't erase the tail, it's the new segment
        }
        place_new_food();
        vga_draw_block(food_x, food_y, COLOR_RED); // Draw new food

        // Update hardware
        update_seven_segment(score);
        if(snake_length <= 10) { // Only light up the 10 LEDs
             set_leds((1 << snake_length) - 1);
        } else {
             set_leds(0x3FF); // All 10 LEDs on
        }
    }

    // --- Update VGA (No-Flicker Method) ---
    if (!ate_food) {
        // Erase the old tail position
        vga_draw_block(old_tail_x, old_tail_y, COLOR_BLACK);
    }
    // Draw the new head
    vga_draw_block(snake_body_x[0], snake_body_y[0], COLOR_CYAN);
    // Draw the new neck (old head position)
    vga_draw_block(snake_body_x[1], snake_body_y[1], COLOR_GREEN);
}


/**
 * @brief Renders the FULL game state.
 * Only called on reset or game over to prevent flicker.
 * (Adapted from your gameover() function)
 */
void render_display() {
    vga_clear_screen(COLOR_BLACK);

    // Draw header separator line
    vga_draw_line(0, GAME_AREA_Y_OFFSET - 1, SCREEN_WIDTH - 1, GAME_AREA_Y_OFFSET - 1, COLOR_WHITE);

    // Draw score (from your gameover() logic)
    char score_str[10];
    score_to_string(score, score_str);
    vga_draw_string(5, 5, "SCORE:", COLOR_WHITE);
    vga_draw_string(45, 5, score_str, COLOR_YELLOW);

    // Draw "SNAKE" title
    vga_draw_string(260, 5, "SNAKE", COLOR_GREEN);

    if (game_over) {
        // (from your gameover() logic)
        vga_draw_string(130, 120, "GAME OVER", COLOR_RED);
        vga_draw_string(90, 130, "FLICK SW9 TO RESET", COLOR_WHITE);
    } else {
        // This is the initial draw (from your update_apple())
        vga_draw_block(food_x, food_y, COLOR_RED);

        // Draw snake (from your showingsnake())
        for (int i = 0; i < snake_length; i++) {
            // Head is Cyan, body is Green
            uint8_t color = (i == 0) ? COLOR_CYAN : COLOR_GREEN;
            vga_draw_block(snake_body_x[i], snake_body_y[i], color);
        }
    }
}