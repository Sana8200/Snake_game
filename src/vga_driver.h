#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H
#include <stdint.h> 

// --- Screen & Game Grid Definitions ---
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define HEADER_HEIGHT 20 // Top area for score/title
#define GAME_AREA_Y_OFFSET HEADER_HEIGHT // Game starts below the header

#define BLOCK_SIZE 16        // Size of one snake/food block in pixels
#define GRID_WIDTH (SCREEN_WIDTH / BLOCK_SIZE)  // 20
#define GRID_HEIGHT ((SCREEN_HEIGHT - HEADER_HEIGHT) / BLOCK_SIZE) // 13
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)


// --- VGA Controller ---
#define VGA_PIXEL_BUFFER_BASE   0x08000000
#define pVGA_PIXEL_BUFFER      ((volatile uint8_t *) VGA_PIXEL_BUFFER_BASE)


// --- VGA Color Definitions (8-bit RGB 3-3-2 format) ---
#define COLOR_BLACK         0x00 // 0b000 000 00
#define COLOR_WHITE         0xFF // 0b111 111 11
#define COLOR_RED           0xE0 // 0b111 000 00
#define COLOR_GREEN         0x1C // 0b000 111 00
#define COLOR_BLUE          0x03 // 0b000 000 11
#define COLOR_YELLOW        0xFC // 0b111 111 00
#define COLOR_CYAN          0x1F // 0b000 111 11
#define COLOR_MAGENTA       0xE3 // 0b111 000 11
#define COLOR_DARK_GRAY     0x49 // 0b100 100 10


// --- Function Prototypes ---
void vga_draw_pixel(int x, int y, uint8_t color);
void vga_clear_screen(uint8_t color);
void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color);
void vga_draw_grid(); // Optional: for drawing a background grid
void vga_repair_grid_column(int x); // Optional: for grid
void vga_draw_string(int x, int y, char *str, uint8_t color);
void vga_draw_block(int grid_x, int grid_y, uint8_t color);

#endif // VGA_DRIVER_H