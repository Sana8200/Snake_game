#include <stddef.h>
#include "snake.h"

// --- Game State Variables ---
static int body_x[MAX_SNAKE_LENGTH];
static int body_y[MAX_SNAKE_LENGTH];
static int length = 0;
static int score = 0;
static int best_score = 0;
static bool game_over = false;
static bool is_paused = false;

static int food_x = 0;
static int food_y = 0;
static char dir = 'r';
static char next_dir = 'r';

// Last state of SW1-SW4, to detect which switch was flipped
static int last_direction_sw = 0;

// The RNG keeps running between games, and the number of input polls
// (which depends on when the player resets) is mixed in at each reset
static unsigned long rng_seed = 1;
static unsigned int poll_count = 0;

// --- HUD Layout ---
#define SCORE_X        45
#define HUD_TEXT_Y     5
#define CONTROLS_X     130

// Pause and game over messages are drawn in a box in the middle of the play
// area, lined up with the grid so it covers whole cells
#define MSG_BOX_X      (5 * BLOCK_SIZE)
#define MSG_BOX_Y      (HEADER_HEIGHT + 5 * BLOCK_SIZE)
#define MSG_BOX_WIDTH  (10 * BLOCK_SIZE)
#define MSG_BOX_HEIGHT (3 * BLOCK_SIZE)

static int rand_range(int min, int max)
{
    rng_seed = rng_seed * 1103515245 + 12345;
    unsigned int r = (rng_seed >> 16) & 0x7FFF;
    return min + (r % (max - min));
}

// Writes str to buf and returns a pointer to the terminating '\0'
static char *write_str(char *buf, const char *str)
{
    while (*str)
        *buf++ = *str++;
    *buf = '\0';
    return buf;
}

// Writes value in decimal to buf and returns a pointer to the terminating '\0'
static char *write_int(char *buf, int value)
{
    char digits[12];
    int n = 0;

    do
    {
        digits[n++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (n > 0)
        *buf++ = digits[--n];
    *buf = '\0';
    return buf;
}

static void draw_centered(int y, const char *str, uint8_t color)
{
    int n = 0;
    while (str[n])
        n++;
    vga_draw_string((SCREEN_WIDTH - n * CHAR_WIDTH) / 2, y, str, color);
}

// detail may be NULL
static void show_message(const char *title, uint8_t title_color, const char *detail, const char *hint)
{
    int left = MSG_BOX_X;
    int right = MSG_BOX_X + MSG_BOX_WIDTH - 1;
    int top = MSG_BOX_Y;
    int bottom = MSG_BOX_Y + MSG_BOX_HEIGHT - 1;

    vga_fill_rect(MSG_BOX_X, MSG_BOX_Y, MSG_BOX_WIDTH, MSG_BOX_HEIGHT, COLOR_BLACK);
    vga_draw_line(left, top, right, top, COLOR_WHITE);
    vga_draw_line(left, bottom, right, bottom, COLOR_WHITE);
    vga_draw_line(left, top, left, bottom, COLOR_WHITE);
    vga_draw_line(right, top, right, bottom, COLOR_WHITE);

    if (detail)
    {
        draw_centered(MSG_BOX_Y + 10, title, title_color);
        draw_centered(MSG_BOX_Y + 20, detail, COLOR_YELLOW);
        draw_centered(MSG_BOX_Y + 30, hint, COLOR_WHITE);
    }
    else
    {
        draw_centered(MSG_BOX_Y + 14, title, title_color);
        draw_centered(MSG_BOX_Y + 26, hint, COLOR_WHITE);
    }
}

static void draw_score_hud(void)
{
    char buf[12];
    write_int(buf, score);

    // Text is drawn without a background, so clear the old score first
    vga_fill_rect(SCORE_X, HUD_TEXT_Y, 5 * CHAR_WIDTH, CHAR_HEIGHT, COLOR_BLACK);
    vga_draw_string(SCORE_X, HUD_TEXT_Y, buf, COLOR_YELLOW);

    set_display(0, score % 10);
    set_display(1, (score / 10) % 10);
    set_display(2, (score / 100) % 10);
    set_display(3, (score / 1000) % 10);
}

static void draw_snake(void)
{
    for (int i = 0; i < length; i++)
        vga_draw_sprite_directional(body_x[i], body_y[i], dir, i == 0);
}

// LEDs 0-9 show the snake length
static void update_leds(void)
{
    set_leds((length >= 10) ? 0x3FF : ((1 << length) - 1));
}

static void spawn_food(void)
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

static void end_game(void)
{
    char detail[32];

    game_over = true;
    if (score > best_score)
    {
        best_score = score;
        write_int(write_str(detail, "NEW BEST: "), score);
    }
    else
    {
        char *p = write_int(write_str(detail, "SCORE: "), score);
        write_int(write_str(p, "  BEST: "), best_score);
    }
    show_message("GAME OVER", COLOR_RED, detail, "SW9 TO RESTART");
}

void snake_init(void)
{
    score = 0;
    length = 5;
    dir = 'r';
    next_dir = 'r';
    game_over = false;
    is_paused = false;

    int sw = get_sw();
    // Switches already on at reset shouldn't count as a flip
    last_direction_sw = sw & DIRECTION_SWITCH_MASK;
    rng_seed += sw + poll_count;

    int start_x = 10;
    int start_y = 6;
    for (int i = 0; i < length; i++)
    {
        body_x[i] = start_x - i;
        body_y[i] = start_y;
    }

    vga_clear_screen(COLOR_BLACK);

    // Draw HUD with control instructions
    vga_draw_string(5, HUD_TEXT_Y, "SCORE:", COLOR_WHITE);
    vga_draw_string(CONTROLS_X, HUD_TEXT_Y, "SW1234:MOVE SW0:PAUSE SW9:RESET", COLOR_YELLOW);
    vga_draw_line(0, HEADER_HEIGHT - 1, SCREEN_WIDTH - 1, HEADER_HEIGHT - 1, COLOR_WHITE);

    draw_score_hud();
    spawn_food();
    draw_snake();
    update_leds();
}

void snake_update_input(void)
{
    poll_count++;

    if (game_over)
        return;

    int sw = get_sw();

    // The game is paused while SW0 is on
    bool pause_now = (sw & PAUSE_SWITCH_BIT) != 0;
    if (pause_now != is_paused)
    {
        is_paused = pause_now;
        if (is_paused)
        {
            show_message("PAUSED", COLOR_YELLOW, NULL, "SW0 OFF TO RESUME");
        }
        else
        {
            // Clear the message and redraw what it covered
            vga_fill_rect(MSG_BOX_X, MSG_BOX_Y, MSG_BOX_WIDTH, MSG_BOX_HEIGHT, COLOR_BLACK);
            vga_draw_sprite(food_x, food_y, SPRITE_FOOD_APPLE);
            draw_snake();
        }
    }

    if (is_paused)
        return;

    // Flipping SW1-SW4 either way turns the snake in that switch's direction,
    // so it doesn't matter which switches were left on
    int direction_sw = sw & DIRECTION_SWITCH_MASK;
    int flipped = direction_sw ^ last_direction_sw;
    last_direction_sw = direction_sw;

    // The snake can't reverse into itself
    if ((flipped & UP_SWITCH_BIT) && dir != 'd')
    {
        next_dir = 'u';
    }
    else if ((flipped & DOWN_SWITCH_BIT) && dir != 'u')
    {
        next_dir = 'd';
    }
    else if ((flipped & LEFT_SWITCH_BIT) && dir != 'r')
    {
        next_dir = 'l';
    }
    else if ((flipped & RIGHT_SWITCH_BIT) && dir != 'l')
    {
        next_dir = 'r';
    }
}

void snake_update_state(void)
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

    // The tail moves out of the way this step, so it's not checked
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
        end_game();
        return;
    }

    bool ate = (head_x == food_x && head_y == food_y);

    if (ate)
    {
        if (length < MAX_SNAKE_LENGTH)
            length++;
        score++;
    }
    else
    {
        // Erase tail (draw black block)
        vga_draw_block(body_x[length - 1], body_y[length - 1], COLOR_BLACK);
    }

    for (int i = length - 1; i > 0; i--)
    {
        body_x[i] = body_x[i - 1];
        body_y[i] = body_y[i - 1];
    }

    body_x[0] = head_x;
    body_y[0] = head_y;

    if (ate)
    {
        // Done after moving, so the new food can't land on the new head
        draw_score_hud();
        spawn_food();
        update_leds();
    }

    // Old head becomes body
    if (length > 1)
    {
        vga_draw_sprite_directional(body_x[1], body_y[1], dir, false);
    }

    // New head with current direction - eyes will face the right way!
    vga_draw_sprite_directional(body_x[0], body_y[0], dir, true);
}
