#include "vga_driver.h"

// --- Font Data (5x7) ---
static const uint8_t font_5x7[][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x00, 0x60, 0x00, 0x00}, // : (10)
    {0x00, 0x60, 0x60, 0x00, 0x00}, // . (11)
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V (12)
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P (13)
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // S (14)
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M (15)
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C (16)
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H (17)
    {0x3E, 0x41, 0x41, 0x22, 0x1C}, // D (18)
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I (19)
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // G (20)
    {0x7F, 0x01, 0x01, 0x01, 0x7F}, // A (21)
    {0x7F, 0x09, 0x09, 0x09, 0x7F}, // E (22)
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, // O (23)
    {0x7F, 0x08, 0x08, 0x08, 0x07}, // R (24)
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T (25)
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N (26)
    {0x7F, 0x40, 0x40, 0x40, 0x7F}, // K (27)
    {0x0F, 0x10, 0x10, 0x10, 0x0F}, // L (28)
    {0x01, 0x02, 0x04, 0x08, 0x10}, // \ (29)
    {0x00, 0x00, 0x00, 0x00, 0x00}  // Space (30)
};

int get_char_idx(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    // A mapping optimization could be done here, but this switch works:
    switch(c) {
        case ':': return 10;
        case '.': return 11;
        case 'V': return 12; case 'P': return 13; case 'S': return 14;
        case 'M': return 15; case 'C': return 16; case 'H': return 17;
        case 'D': return 18; case 'I': return 19; case 'G': return 20;
        case 'A': return 21; case 'E': return 22; case 'O': return 23;
        case 'R': return 24; case 'T': return 25; case 'N': return 26;
        case 'K': return 27; case 'L': return 28; case '\\': return 29;
        default: return 30; // Space
    }
}

int abs(int n) { return (n < 0) ? -n : n; }

void vga_draw_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        pVGA_PIXEL_BUFFER[(y * SCREEN_WIDTH) + x] = color;
    }
}

void vga_clear_screen(uint8_t color){
    // Simple loop unrolling or block set could speed this up for 
    // performance analysis, but this is fine for now.
    int total = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total; i++) pVGA_PIXEL_BUFFER[i] = color;
}

void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color) {
    // Horizontal optimization
    if (y1 == y2) {
        int start = (x1 < x2) ? x1 : x2;
        int end = (x1 < x2) ? x2 : x1;
        for (int x = start; x <= end; x++) vga_draw_pixel(x, y1, color);
        return;
    }
    // Vertical optimization
    if (x1 == x2) {
        int start = (y1 < y2) ? y1 : y2;
        int end = (y1 < y2) ? y2 : y1;
        for (int y = start; y <= end; y++) vga_draw_pixel(x1, y, color);
        return;
    }
    // Bresenham Algorithm
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        vga_draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void vga_draw_char(int x, int y, char c, uint8_t color) {
    int idx = get_char_idx(c);
    const uint8_t *bitmap = font_5x7[idx];
    for (int col = 0; col < 5; col++) {
        uint8_t data = bitmap[col];
        for (int row = 0; row < 7; row++) {
            if (data & (1 << row)) {
                vga_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void vga_draw_string(int x, int y, char *str, uint8_t color) {
    while (*str) {
        if (*str == '\n') {
            y += 8; x = 5;
        } else {
            vga_draw_char(x, y, *str, color);
            x += 6; 
        }
        str++;
    }
}


void vga_draw_block(int grid_x, int grid_y, uint8_t color) {
    // Convert Grid Coordinate to Pixel Coordinate
    // NOTICE: We add HEADER_HEIGHT to Y so we don't draw over the score!
    int pixel_x = grid_x * BLOCK_SIZE;
    int pixel_y = HEADER_HEIGHT + (grid_y * BLOCK_SIZE);

    if (pixel_x >= SCREEN_WIDTH || pixel_y >= SCREEN_HEIGHT) return;

    // Draw square with 1px border
    for(int y = 0; y < BLOCK_SIZE - 1; y++) {
        for(int x = 0; x < BLOCK_SIZE - 1; x++) {
            vga_draw_pixel(pixel_x + x, pixel_y + y, color);
        }
    }
}