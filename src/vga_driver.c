#include "vga_driver.h"

// --- Fixed Font Data (5x7) ---
// Each character is 5 bytes, each byte represents a column
// Bit 0 = top, Bit 6 = bottom
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
    
    // Special characters
    {0x00, 0x36, 0x36, 0x00, 0x00}, // : (colon) - index 10
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ; (semicolon) - index 11  
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space - index 12
    
    // Letters A-Z (uppercase)
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A - index 13
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B - index 14
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C - index 15
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D - index 16
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E - index 17
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F - index 18
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G - index 19
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H - index 20
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I - index 21
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J - index 22
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K - index 23
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L - index 24
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M - index 25
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N - index 26
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O - index 27
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P - index 28
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q - index 29
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R - index 30
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S - index 31
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T - index 32
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U - index 33
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V - index 34
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W - index 35
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X - index 36
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y - index 37
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z - index 38
};

int get_char_idx(char c) {
    // Numbers 0-9
    if (c >= '0' && c <= '9') {
        return c - '0';  // Returns 0-9
    }
    
    // Uppercase letters A-Z
    if (c >= 'A' && c <= 'Z') {
        return 13 + (c - 'A');  // Returns 13-38
    }
    
    // Lowercase letters a-z (map to uppercase)
    if (c >= 'a' && c <= 'z') {
        return 13 + (c - 'a');  // Returns 13-38
    }
    
    // Special characters
    switch(c) {
        case ':': return 10;
        case ';': return 11;
        case ' ': return 12;
        default: return 12;  // Default to space for unknown chars
    }
}

int abs(int n) { return (n < 0) ? -n : n; }

void vga_draw_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        pVGA_PIXEL_BUFFER[(y * SCREEN_WIDTH) + x] = color;
    }
}

void vga_clear_screen(uint8_t color){
    int total = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total; i++) pVGA_PIXEL_BUFFER[i] = color;
}

void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color) {
    if (y1 == y2) {
        int start = (x1 < x2) ? x1 : x2;
        int end = (x1 < x2) ? x2 : x1;
        for (int x = start; x <= end; x++) vga_draw_pixel(x, y1, color);
        return;
    }
    if (x1 == x2) {
        int start = (y1 < y2) ? y1 : y2;
        int end = (y1 < y2) ? y2 : y1;
        for (int y = start; y <= end; y++) vga_draw_pixel(x1, y, color);
        return;
    }
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
    
    // Draw each column
    for (int col = 0; col < 5; col++) {
        uint8_t data = bitmap[col];
        // Draw each row in the column
        for (int row = 0; row < 7; row++) {
            if (data & (1 << row)) {
                vga_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void vga_draw_string(int x, int y, char *str, uint8_t color) {
    int cur_x = x;
    int cur_y = y;
    
    while (*str) {
        if (*str == '\n') {
            cur_y += 8;
            cur_x = x;  // Return to start X position
        } else {
            vga_draw_char(cur_x, cur_y, *str, color);
            cur_x += 6;  // Move to next character position
        }
        str++;
    }
}

void vga_draw_block(int grid_x, int grid_y, uint8_t color) {
    int pixel_x = grid_x * BLOCK_SIZE;
    int pixel_y = HEADER_HEIGHT + (grid_y * BLOCK_SIZE);

    if (pixel_x >= SCREEN_WIDTH || pixel_y >= SCREEN_HEIGHT) return;

    for(int y = 0; y < BLOCK_SIZE - 1; y++) {
        for(int x = 0; x < BLOCK_SIZE - 1; x++) {
            vga_draw_pixel(pixel_x + x, pixel_y + y, color);
        }
    }
}

// ============= SPRITE FUNCTIONS =============

// Helper: Draw a filled circle (for eyes, apple)
static void draw_circle_filled(int cx, int cy, int radius, uint8_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                vga_draw_pixel(cx + x, cy + y, color);
            }
        }
    }
}

// Draw snake head with eyes and direction
static void draw_snake_head(int px, int py, char direction) {
    // Background - head color
    for(int y = 1; y < 15; y++) {
        for(int x = 1; x < 15; x++) {
            vga_draw_pixel(px + x, py + y, COLOR_CYAN);
        }
    }
    
    // Border/outline
    for(int x = 2; x < 14; x++) {
        vga_draw_pixel(px + x, py + 1, COLOR_DARK_GREEN);
        vga_draw_pixel(px + x, py + 14, COLOR_DARK_GREEN);
    }
    for(int y = 2; y < 14; y++) {
        vga_draw_pixel(px + 1, py + y, COLOR_DARK_GREEN);
        vga_draw_pixel(px + 14, py + y, COLOR_DARK_GREEN);
    }
    
    // Eyes position based on direction
    int eye1_x, eye1_y, eye2_x, eye2_y;
    
    switch(direction) {
        case 'u': // Up - eyes on top
            eye1_x = 5; eye1_y = 4;
            eye2_x = 10; eye2_y = 4;
            break;
        case 'd': // Down - eyes on bottom
            eye1_x = 5; eye1_y = 11;
            eye2_x = 10; eye2_y = 11;
            break;
        case 'l': // Left - eyes on left
            eye1_x = 4; eye1_y = 5;
            eye2_x = 4; eye2_y = 10;
            break;
        case 'r': // Right - eyes on right
        default:
            eye1_x = 11; eye1_y = 5;
            eye2_x = 11; eye2_y = 10;
            break;
    }
    
    // Draw eyes (white with black pupils)
    draw_circle_filled(px + eye1_x, py + eye1_y, 2, COLOR_WHITE);
    draw_circle_filled(px + eye2_x, py + eye2_y, 2, COLOR_WHITE);
    vga_draw_pixel(px + eye1_x, py + eye1_y, COLOR_BLACK);
    vga_draw_pixel(px + eye2_x, py + eye2_y, COLOR_BLACK);
}

// Draw snake body segment with scale pattern
static void draw_snake_body(int px, int py) {
    // Base body color
    for(int y = 1; y < 15; y++) {
        for(int x = 1; x < 15; x++) {
            vga_draw_pixel(px + x, py + y, COLOR_GREEN);
        }
    }
    
    // Scale pattern (diagonal lines)
    for(int i = 0; i < 16; i += 4) {
        for(int j = 0; j < 3; j++) {
            if (i + j < 15) {
                vga_draw_pixel(px + 2 + i + j, py + 2 + j, COLOR_LIGHT_GREEN);
                vga_draw_pixel(px + 2 + i + j, py + 8 + j, COLOR_LIGHT_GREEN);
            }
        }
    }
    
    // Border
    for(int x = 2; x < 14; x++) {
        vga_draw_pixel(px + x, py + 1, COLOR_DARK_GREEN);
        vga_draw_pixel(px + x, py + 14, COLOR_DARK_GREEN);
    }
    for(int y = 2; y < 14; y++) {
        vga_draw_pixel(px + 1, py + y, COLOR_DARK_GREEN);
        vga_draw_pixel(px + 14, py + y, COLOR_DARK_GREEN);
    }
}

// Draw apple/food
static void draw_apple(int px, int py) {
    // Clear background
    for(int y = 0; y < 16; y++) {
        for(int x = 0; x < 16; x++) {
            vga_draw_pixel(px + x, py + y, COLOR_BLACK);
        }
    }
    
    // Red apple body
    draw_circle_filled(px + 8, py + 9, 5, COLOR_RED);
    
    // Stem (brown/dark)
    for(int i = 0; i < 3; i++) {
        vga_draw_pixel(px + 8, py + 3 + i, COLOR_DARK_GREEN);
    }
    
    // Leaf (green)
    vga_draw_pixel(px + 9, py + 4, COLOR_GREEN);
    vga_draw_pixel(px + 10, py + 4, COLOR_GREEN);
    vga_draw_pixel(px + 10, py + 5, COLOR_GREEN);
    
    // Highlight on apple
    vga_draw_pixel(px + 6, py + 7, COLOR_WHITE);
    vga_draw_pixel(px + 7, py + 7, COLOR_WHITE);
    vga_draw_pixel(px + 6, py + 8, COLOR_WHITE);
}

// Main sprite drawing function
void vga_draw_sprite(int grid_x, int grid_y, SpriteType sprite) {
    int pixel_x = grid_x * BLOCK_SIZE;
    int pixel_y = HEADER_HEIGHT + (grid_y * BLOCK_SIZE);
    
    if (pixel_x >= SCREEN_WIDTH || pixel_y >= SCREEN_HEIGHT) return;
    
    switch(sprite) {
        case SPRITE_SNAKE_HEAD_UP:
            draw_snake_head(pixel_x, pixel_y, 'u');
            break;
        case SPRITE_SNAKE_HEAD_DOWN:
            draw_snake_head(pixel_x, pixel_y, 'd');
            break;
        case SPRITE_SNAKE_HEAD_LEFT:
            draw_snake_head(pixel_x, pixel_y, 'l');
            break;
        case SPRITE_SNAKE_HEAD_RIGHT:
            draw_snake_head(pixel_x, pixel_y, 'r');
            break;
        case SPRITE_FOOD_APPLE:
            draw_apple(pixel_x, pixel_y);
            break;
        default:
            draw_snake_body(pixel_x, pixel_y);
            break;
    }
}

// Convenient directional sprite function
void vga_draw_sprite_directional(int grid_x, int grid_y, char direction, bool is_head) {
    if (is_head) {
        switch(direction) {
            case 'u': vga_draw_sprite(grid_x, grid_y, SPRITE_SNAKE_HEAD_UP); break;
            case 'd': vga_draw_sprite(grid_x, grid_y, SPRITE_SNAKE_HEAD_DOWN); break;
            case 'l': vga_draw_sprite(grid_x, grid_y, SPRITE_SNAKE_HEAD_LEFT); break;
            case 'r': vga_draw_sprite(grid_x, grid_y, SPRITE_SNAKE_HEAD_RIGHT); break;
        }
    } else {
        draw_snake_body(grid_x * BLOCK_SIZE, HEADER_HEIGHT + (grid_y * BLOCK_SIZE));
    }
}