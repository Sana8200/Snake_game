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

// Track last switch state to detect edges
static int last_direction_sw = 0;

static unsigned long rng_seed = 1;

int rand_range(int min, int max)
{
    rng_seed = rng_seed * 1103515245 + 12345;
    unsigned int r = (rng_seed >> 16) & 0x7FFF;
    return min + (r % (max - min));
}

void draw_score_hud()
{
    char buf[16];
    int temp = score;
    int i = 0;

    if (temp == 0)
    {
        buf[i++] = '0';
    }
    else
    {
        while (temp > 0)
        {
            buf[i++] = (temp % 10) + '0';
            temp /= 10;
        }
    }
    buf[i] = '\0';

    for (int j = 0; j < i / 2; j++)
    {
        char t = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = t;
    }

    vga_draw_string(45, 5, "     ", COLOR_BLACK);
    vga_draw_string(45, 5, buf, COLOR_YELLOW);

    set_display(0, score % 10);
    set_display(1, (score / 10) % 10);
    set_display(2, (score / 100) % 10);
    set_display(3, (score / 1000) % 10);
}

void spawn_food()
{
    bool valid = false;
    int attempts = 0;

    if (length >= MAX_SNAKE_LENGTH - 1)
        return;

    while (!valid && attempts < 500)
    {
        food_x = rand_range(0, GRID_WIDTH);
        food_y = rand_range(0, GRID_HEIGHT);
        valid = true;

        for (int i = 0; i < length; i++)
        {
            if (body_x[i] == food_x && body_y[i] == food_y)
            {
                valid = false;
                break;
            }
        }
        attempts++;
    }

    // Draw apple sprite for food
    vga_draw_sprite(food_x, food_y, SPRITE_FOOD_APPLE);
}

void snake_init()
{
    score = 0;
    length = 5;
    dir = 'r';
    next_dir = 'r';
    game_over = false;
    is_paused = false;
    last_direction_sw = 0;

    rng_seed = get_sw() + 12345;

    int start_x = 10;
    int start_y = 6;
    for (int i = 0; i < length; i++)
    {
        body_x[i] = start_x - i;
        body_y[i] = start_y;
    }

    vga_clear_screen(COLOR_BLACK);

    // Draw HUD with control instructions
    vga_draw_string(5, 5, "SCORE:", COLOR_WHITE);
    vga_draw_string(140, 5, "SW1234:MOVE SW0:PAUSE SW9:RESET", COLOR_YELLOW);
    vga_draw_line(0, HEADER_HEIGHT - 1, SCREEN_WIDTH - 1, HEADER_HEIGHT - 1, COLOR_WHITE);

    draw_score_hud();
    spawn_food();

    // Draw initial snake with sprites
    for (int i = 0; i < length; i++)
    {
        if (i == 0)
        {
            // Head sprite with direction
            vga_draw_sprite_directional(body_x[i], body_y[i], dir, true);
        }
        else
        {
            // Body sprite
            vga_draw_sprite_directional(body_x[i], body_y[i], dir, false);
        }
    }

    int led_mask = (length >= 10) ? 0x3FF : ((1 << length) - 1);
    set_leds(led_mask);
}

void snake_update_input()
{
    if (game_over)
        return;

    int sw = get_sw();

    // Pause toggle (SW0)
    bool pause_now = (sw & PAUSE_SWITCH_BIT);
    if (pause_now && !last_pause_state)
    {
        is_paused = !is_paused;
        if (is_paused)
        {
            vga_draw_string(120, 100, "PAUSED", COLOR_YELLOW);
            vga_draw_string(80, 120, "FLIP SW0 TO RESUME", COLOR_WHITE);
        }
        else
        {
            vga_draw_string(120, 100, "      ", COLOR_BLACK);
            vga_draw_string(80, 120, "                  ", COLOR_BLACK);

            // Redraw food sprite
            vga_draw_sprite(food_x, food_y, SPRITE_FOOD_APPLE);

            // Redraw snake with sprites
            for (int i = 0; i < length; i++)
            {
                if (i == 0)
                {
                    vga_draw_sprite_directional(body_x[i], body_y[i], dir, true);
                }
                else
                {
                    vga_draw_sprite_directional(body_x[i], body_y[i], dir, false);
                }
            }
        }
    }
    last_pause_state = pause_now;

    if (is_paused)
        return;

    if (get_btn)
    {
        return;
    }

    // Direction control using SW1-SW4 (bits 1-4)
    // Extract direction switches
    int direction_sw = (sw >> 1) & 0x0F; // Get bits 1-4

    // Only change direction when a switch changes state (edge detection)
    // This prevents continuous direction changes while switch is held
    if (direction_sw != last_direction_sw && direction_sw != 0)
    {
        // SW1 (bit 1) = UP
        if ((direction_sw & 0x01) && dir != 'd')
        {
            next_dir = 'u';
        }
        // SW2 (bit 2) = DOWN
        else if ((direction_sw & 0x02) && dir != 'u')
        {
            next_dir = 'd';
        }
        // SW3 (bit 3) = LEFT
        else if ((direction_sw & 0x04) && dir != 'r')
        {
            next_dir = 'l';
        }
        // SW4 (bit 4) = RIGHT
        else if ((direction_sw & 0x08) && dir != 'l')
        {
            next_dir = 'r';
        }
    }

    last_direction_sw = direction_sw;
}

void snake_update_state()
{
    if (game_over || is_paused)
        return;

    dir = next_dir;
    int head_x = body_x[0];
    int head_y = body_y[0];

    if (dir == 'u')
        head_y--;
    if (dir == 'd')
        head_y++;
    if (dir == 'l')
        head_x--;
    if (dir == 'r')
        head_x++;

    bool deadly_walls = (get_sw() & WALL_MODE_SWITCH_BIT);
    bool hit_wall = false;

    if (head_x < 0)
    {
        if (deadly_walls)
            hit_wall = true;
        else
            head_x = GRID_WIDTH - 1;
    }
    else if (head_x >= GRID_WIDTH)
    {
        if (deadly_walls)
            hit_wall = true;
        else
            head_x = 0;
    }

    if (head_y < 0)
    {
        if (deadly_walls)
            hit_wall = true;
        else
            head_y = GRID_HEIGHT - 1;
    }
    else if (head_y >= GRID_HEIGHT)
    {
        if (deadly_walls)
            hit_wall = true;
        else
            head_y = 0;
    }

    bool hit_self = false;
    for (int i = 0; i < length - 1; i++)
    {
        if (body_x[i] == head_x && body_y[i] == head_y)
        {
            hit_self = true;
            break;
        }
    }

    if (hit_wall || hit_self)
    {
        game_over = true;
        vga_draw_string(100, 110, "GAME OVER", COLOR_RED);
        vga_draw_string(80, 130, "SW9 TO RESTART", COLOR_WHITE);
        return;
    }

    bool ate = (head_x == food_x && head_y == food_y);

    if (!ate)
    {
        // Erase tail (draw black block)
        vga_draw_block(body_x[length - 1], body_y[length - 1], COLOR_BLACK);
    }
    else
    {
        if (length < MAX_SNAKE_LENGTH)
            length++;
        score++;
        draw_score_hud();
        spawn_food();

        int led_mask = (length >= 10) ? 0x3FF : ((1 << length) - 1);
        set_leds(led_mask);
    }

    for (int i = length - 1; i > 0; i--)
    {
        body_x[i] = body_x[i - 1];
        body_y[i] = body_y[i - 1];
    }

    body_x[0] = head_x;
    body_y[0] = head_y;

    // Draw updated snake with sprites!
    // Old head becomes body
    if (length > 1)
    {
        vga_draw_sprite_directional(body_x[1], body_y[1], dir, false);
    }

    // New head with current direction - eyes will face the right way!
    vga_draw_sprite_directional(body_x[0], body_y[0], dir, true);
}