#include "snake.h"

// --- Game State Variables ---
int body_x[MAX_SNAKE_LENGTH];
int body_y[MAX_SNAKE_LENGTH];
int length = 0;
int score = 0;
bool game_over = false;
bool is_paused = false;

int food_x = 0;
int food_y = 0;
char dir = 'r';
char next_dir = 'r';
bool last_pause_state = false;

static unsigned long rng_seed = 1;

int rand_range(int min, int max) {
    rng_seed = rng_seed * 1103515245 + 12345;
    unsigned int r = (rng_seed >> 16) & 0x7FFF;
    return min + (r % (max - min));
}

void draw_score_hud() {
    char buf[16];
    int temp = score;
    int i = 0;

    if (temp == 0) buf[i++] = '0';
    while (temp > 0) {
        buf[i++] = (temp % 10) + '0';
        temp /= 10;
    }
    buf[i] = '\0';

    // String Reverse
    for (int j = 0; j < i / 2; j++) {
        char t = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = t;
    }

    // Clear previous score area (small block)
    // Using draw_block isn't precise enough for text, so we just overwrite with text
    // Ideally draw a black rectangle behind the text, but this works:
    vga_draw_string(45, 5, "     ", COLOR_BLACK); 
    vga_draw_string(45, 5, buf, COLOR_YELLOW);

    // 7-Segment Display
    set_display(0, score % 10);
    set_display(1, (score / 10) % 10);
    set_display(2, (score / 100) % 10);
    set_display(3, (score / 1000) % 10);
}

void spawn_food() {
    bool valid = false;
    int attempts = 0;
    
    // Safety: If snake fills screen, don't hang forever
    if (length >= MAX_SNAKE_LENGTH - 1) return;

    while (!valid && attempts < 500) {
        food_x = rand_range(0, GRID_WIDTH);
        food_y = rand_range(0, GRID_HEIGHT);
        valid = true;
        
        // Check collision with snake body
        for (int i = 0; i < length; i++) {
            if (body_x[i] == food_x && body_y[i] == food_y) {
                valid = false;
                break;
            }
        }
        attempts++;
    }
    vga_draw_block(food_x, food_y, COLOR_RED);
}

void snake_init() {
    score = 0;
    length = 5;
    dir = 'r';
    next_dir = 'r';
    game_over = false;
    is_paused = false;

    // Seed RNG using switches + a constant so it changes if you flip switches
    rng_seed = get_sw() + 12345;

    // Center Snake
    int start_x = 10;
    int start_y = 6;
    for (int i = 0; i < length; i++) {
        body_x[i] = start_x - i;
        body_y[i] = start_y;
    }

    vga_clear_screen(COLOR_BLACK);

    // Draw HUD
    vga_draw_string(5, 5, "SCORE:", COLOR_WHITE);
    vga_draw_string(260, 5, "SNAKE", COLOR_GREEN);
    vga_draw_line(0, HEADER_HEIGHT - 1, SCREEN_WIDTH - 1, HEADER_HEIGHT - 1, COLOR_WHITE);

    draw_score_hud();
    spawn_food();

    // Draw Initial Snake
    for (int i = 0; i < length; i++) {
        vga_draw_block(body_x[i], body_y[i], (i == 0) ? COLOR_CYAN : COLOR_GREEN);
    }
    set_leds((1 << length) - 1);
}

void snake_update_input() {
    if (game_over) return;

    int sw = get_sw();
    int btn = get_btn(); // Assuming active LOW (0 = pressed)

    // Pause Toggle Logic
    bool pause_now = (sw & PAUSE_SWITCH_BIT);
    if (pause_now && !last_pause_state) {
        is_paused = !is_paused;
        if (is_paused) {
            vga_draw_string(140, 120, "PAUSED", COLOR_YELLOW);
        } else {
            // Redraw game state to clear "PAUSED" text
            vga_draw_string(140, 120, "      ", COLOR_BLACK); 
            vga_draw_block(food_x, food_y, COLOR_RED);
            for (int i = 0; i < length; i++)
                vga_draw_block(body_x[i], body_y[i], (i == 0) ? COLOR_CYAN : COLOR_GREEN);
        }
    }
    last_pause_state = pause_now;

    if (is_paused) return;

    if(!(btn))
    if (!(btn & KEY_UP_BIT)    && dir != 'd') next_dir = 'u';
    if (!(btn & KEY_DOWN_BIT)  && dir != 'u') next_dir = 'd';
    if (!(btn & KEY_LEFT_BIT)  && dir != 'r') next_dir = 'l';
    if (!(btn & KEY_RIGHT_BIT) && dir != 'l') next_dir = 'r';
}

void snake_update_state() {
    if (game_over || is_paused) return;

    dir = next_dir;
    int head_x = body_x[0];
    int head_y = body_y[0];

    if (dir == 'u') head_y--;
    if (dir == 'd') head_y++;
    if (dir == 'l') head_x--;
    if (dir == 'r') head_x++;

    // Wall Collision Logic
    bool deadly_walls = (get_sw() & WALL_MODE_SWITCH_BIT);
    bool hit_wall = false;

    if (head_x < 0) {
        if (deadly_walls) hit_wall = true;
        else head_x = GRID_WIDTH - 1;
    }
    else if (head_x >= GRID_WIDTH) {
        if (deadly_walls) hit_wall = true;
        else head_x = 0;
    }

    if (head_y < 0) {
        if (deadly_walls) hit_wall = true;
        else head_y = GRID_HEIGHT - 1;
    }
    else if (head_y >= GRID_HEIGHT) {
        if (deadly_walls) hit_wall = true;
        else head_y = 0;
    }

    // Self Collision
    bool hit_self = false;
    for (int i = 0; i < length - 1; i++) {
        if (body_x[i] == head_x && body_y[i] == head_y) {
            hit_self = true;
            break;
        }
    }

    if (hit_wall || hit_self) {
        game_over = true;
        vga_draw_string(130, 120, "GAME OVER", COLOR_RED);
        return;
    }

    // Move Logic
    bool ate = (head_x == food_x && head_y == food_y);

    if (!ate) {
        // Erase tail
        vga_draw_block(body_x[length - 1], body_y[length - 1], COLOR_BLACK);
    } else {
        if (length < MAX_SNAKE_LENGTH) length++;
        score++;
        draw_score_hud();
        spawn_food();
        // Update LEDs
        int mask = (length >= 10) ? 0x3FF : ((1 << length) - 1);
        set_leds(mask);
    }

    // Shift Body
    for (int i = length - 1; i > 0; i--) {
        body_x[i] = body_x[i - 1];
        body_y[i] = body_y[i - 1];
    }

    // Update Head
    body_x[0] = head_x;
    body_y[0] = head_y;

    // Draw new segments
    vga_draw_block(body_x[1], body_y[1], COLOR_GREEN); // Old head becomes body
    vga_draw_block(body_x[0], body_y[0], COLOR_CYAN);  // New head
}