#include <stdint.h>
#include "console.h"
#include "../../hal/io.h"
#include "../../drivers/keyboard/keyboard.h"

#define VGA_ADDR 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define COLOR_LIGHT_GREY 7
#define COLOR_BLACK 0

static uint16_t *const vga_buffer = (uint16_t *)VGA_ADDR;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t vga_color = (COLOR_BLACK << 4) | COLOR_LIGHT_GREY;

/* Simple ring buffer for keyboard input */
#define INPUT_BUF_SIZE 128
static char input_buf[INPUT_BUF_SIZE];
static unsigned int input_head = 0;
static unsigned int input_tail = 0;

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

/* Poll keyboard driver for new input */
void console_poll_input(void) {
    char c = keyboard_read_char();
    if (!c)
        return;
    unsigned int next_head = (input_head + 1) % INPUT_BUF_SIZE;
    if (next_head != input_tail) {
        input_buf[input_head] = c;
        input_head = next_head;
    }
}

/* Retrieve a character from the input buffer */
char console_getc(void) {
    if (input_head == input_tail)
        return 0;
    char c = input_buf[input_tail];
    input_tail = (input_tail + 1) % INPUT_BUF_SIZE;
    return c;
}
