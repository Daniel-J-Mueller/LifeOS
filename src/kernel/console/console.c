#include <stdint.h>
#include "console.h"
#include "../../hal/io.h"

#define VGA_ADDR 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define COLOR_LIGHT_GREY 7
#define COLOR_BLACK 0

static uint16_t *const vga_buffer = (uint16_t *)VGA_ADDR;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t vga_color = (COLOR_BLACK << 4) | COLOR_LIGHT_GREY;

static void update_cursor(void) {
    uint16_t pos = cursor_y * VGA_COLS + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void console_clear(void) {
    for (unsigned int i = 0; i < VGA_COLS * VGA_ROWS; ++i)
        vga_buffer[i] = ((uint16_t)vga_color << 8) | ' ';
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void console_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        if (++cursor_y >= VGA_ROWS)
            cursor_y = 0;
    } else {
        vga_buffer[cursor_y * VGA_COLS + cursor_x] = ((uint16_t)vga_color << 8) | c;
        if (++cursor_x >= VGA_COLS) {
            cursor_x = 0;
            if (++cursor_y >= VGA_ROWS)
                cursor_y = 0;
        }
    }
    update_cursor();
}

void console_write(const char *s) {
    while (*s)
        console_putc(*s++);
}

void console_init(void) {
    console_clear();
}
