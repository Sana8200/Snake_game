#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include <stdint.h> 
#include <stdbool.h>

// --- Screen Dimensions ---
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// --- Game Layout Configuration ---
#define HEADER_HEIGHT 20         // Height of the top score bar
#define BLOCK_SIZE    16         // Size of one snake block

// --- Grid Calculations ---
#define GRID_WIDTH    (SCREEN_WIDTH / BLOCK_SIZE)
#define GRID_HEIGHT   ((SCREEN_HEIGHT - HEADER_HEIGHT) / BLOCK_SIZE)

// --- Colors (8-bit RGB 3-3-2) ---
#define COLOR_BLACK         0x00
#define COLOR_WHITE         0xFF
#define COLOR_RED           0xE0
#define COLOR_GREEN         0x1C
#define COLOR_BLUE          0x03
#define COLOR_YELLOW        0xFC
#define COLOR_CYAN          0x1F
#define COLOR_MAGENTA       0xE3
#define COLOR_DARK_GRAY     0x49
#define COLOR_LIGHT_GREEN   0x9C
#define COLOR_DARK_GREEN    0x14
#define COLOR_ORANGE        0xE4

// --- VGA Base Address ---
#define VGA_PIXEL_BUFFER_BASE 0x08000000
#define pVGA_PIXEL_BUFFER    ((volatile uint8_t *) VGA_PIXEL_BUFFER_BASE)

// --- Sprite Types ---
typedef enum {
    SPRITE_SNAKE_HEAD_UP,
    SPRITE_SNAKE_HEAD_DOWN,
    SPRITE_SNAKE_HEAD_LEFT,
    SPRITE_SNAKE_HEAD_RIGHT,
    SPRITE_SNAKE_BODY_HORIZONTAL,
    SPRITE_SNAKE_BODY_VERTICAL,
    SPRITE_SNAKE_BODY_CORNER_TL,  // Top-Left corner
    SPRITE_SNAKE_BODY_CORNER_TR,  // Top-Right corner
    SPRITE_SNAKE_BODY_CORNER_BL,  // Bottom-Left corner
    SPRITE_SNAKE_BODY_CORNER_BR,  // Bottom-Right corner
    SPRITE_SNAKE_TAIL_UP,
    SPRITE_SNAKE_TAIL_DOWN,
    SPRITE_SNAKE_TAIL_LEFT,
    SPRITE_SNAKE_TAIL_RIGHT,
    SPRITE_FOOD_APPLE
} SpriteType;

// --- Prototypes ---
void vga_clear_screen(uint8_t color);
void vga_draw_pixel(int x, int y, uint8_t color);
void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color);
void vga_draw_string(int x, int y, char *str, uint8_t color);
void vga_draw_block(int grid_x, int grid_y, uint8_t color);

// New sprite functions
void vga_draw_sprite(int grid_x, int grid_y, SpriteType sprite);
void vga_draw_sprite_directional(int grid_x, int grid_y, char direction, bool is_head);

#endif