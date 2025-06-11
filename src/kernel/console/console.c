#include <stdint.h>
#include "console.h"
#include "../../hal/io.h"
#include "../../drivers/keyboard/keyboard.h"
#include "../hmi/pane.h"
#include "shell.h"

#define SERIAL_PORT 0x3F8

#define VGA_ADDR 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define SCROLLBACK_LINES 100
#define COLOR_LIGHT_GREY 7
#define COLOR_BLACK 0

static volatile uint16_t *const vga_buffer = (volatile uint16_t *)VGA_ADDR;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t vga_color = (COLOR_BLACK << 4) | COLOR_LIGHT_GREY;

/* In-memory copies of the visible screen and additional scrollback lines */
static uint16_t screen_buffer[VGA_ROWS][VGA_COLS];
static uint16_t scrollback[SCROLLBACK_LINES][VGA_COLS];
static unsigned int sb_start = 0;   /* index of oldest line */
static unsigned int sb_count = 0;   /* number of stored lines */
static unsigned int sb_view = 0;    /* lines above bottom currently shown */

static void refresh_screen(void) {
    if (sb_view)
        return;
    for (unsigned int y = 0; y < VGA_ROWS; ++y)
        for (unsigned int x = 0; x < VGA_COLS; ++x)
            vga_buffer[y * VGA_COLS + x] = screen_buffer[y][x];
    update_cursor();
}

/* Simple ring buffer for keyboard input */
#define INPUT_BUF_SIZE 128
static char input_buf[INPUT_BUF_SIZE];
static unsigned int input_head = 0;
static unsigned int input_tail = 0;

static void serial_init(void) {
    outb(SERIAL_PORT + 1, 0x00);    /* Disable interrupts */
    outb(SERIAL_PORT + 3, 0x80);    /* Enable DLAB */
    outb(SERIAL_PORT + 0, 0x01);    /* Divisor low byte (115200) */
    outb(SERIAL_PORT + 1, 0x00);    /* Divisor high byte */
    outb(SERIAL_PORT + 3, 0x03);    /* 8 bits, no parity, one stop */
    outb(SERIAL_PORT + 2, 0xC7);    /* Enable FIFO */
    outb(SERIAL_PORT + 4, 0x0B);    /* IRQs disabled, RTS/DSR set */
}

static void serial_putc(char c) {
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0)
        ;
    outb(SERIAL_PORT, c);
}

static void update_cursor(void) {
    uint16_t pos = cursor_y * VGA_COLS + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

static void scrollback_push(const uint16_t *line) {
    unsigned int idx = (sb_start + sb_count) % SCROLLBACK_LINES;
    for (unsigned int x = 0; x < VGA_COLS; ++x)
        scrollback[idx][x] = line[x];
    if (sb_count < SCROLLBACK_LINES)
        sb_count++;
    else
        sb_start = (sb_start + 1) % SCROLLBACK_LINES;
}

static void render_view(void) {
    unsigned int total = sb_count + VGA_ROWS;
    if (sb_view > sb_count)
        sb_view = sb_count;
    unsigned int start = (total > VGA_ROWS + sb_view) ?
        total - VGA_ROWS - sb_view : 0;

    for (unsigned int row = 0; row < VGA_ROWS; ++row) {
        unsigned int idx = start + row;
        if (idx < sb_count) {
            unsigned int real = (sb_start + idx) % SCROLLBACK_LINES;
            for (unsigned int x = 0; x < VGA_COLS; ++x)
                vga_buffer[row * VGA_COLS + x] = scrollback[real][x];
        } else {
            unsigned int scr = idx - sb_count;
            if (scr < VGA_ROWS)
                for (unsigned int x = 0; x < VGA_COLS; ++x)
                    vga_buffer[row * VGA_COLS + x] = screen_buffer[scr][x];
            else
                for (unsigned int x = 0; x < VGA_COLS; ++x)
                    vga_buffer[row * VGA_COLS + x] = ((uint16_t)vga_color << 8) | ' ';
        }
    }
    update_cursor();
}

static void scroll_screen(void) {
    scrollback_push(screen_buffer[0]);
    for (unsigned int y = 0; y < VGA_ROWS - 1; ++y)
        for (unsigned int x = 0; x < VGA_COLS; ++x)
            screen_buffer[y][x] = screen_buffer[y + 1][x];
    for (unsigned int x = 0; x < VGA_COLS; ++x)
        screen_buffer[VGA_ROWS - 1][x] = ((uint16_t)vga_color << 8) | ' ';
    if (sb_view == 0)
        for (unsigned int y = 0; y < VGA_ROWS; ++y)
            for (unsigned int x = 0; x < VGA_COLS; ++x)
                vga_buffer[y * VGA_COLS + x] = screen_buffer[y][x];
}

static void ensure_bottom(void) {
    if (sb_view) {
        sb_view = 0;
        render_view();
    }
}

void console_clear(void) {
    for (unsigned int y = 0; y < VGA_ROWS; ++y)
        for (unsigned int x = 0; x < VGA_COLS; ++x) {
            uint16_t val = ((uint16_t)vga_color << 8) | ' ';
            screen_buffer[y][x] = val;
            vga_buffer[y * VGA_COLS + x] = val;
        }
    sb_start = sb_count = sb_view = 0;
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void console_putc(char c) {
    serial_putc(c);
    ensure_bottom();
    if (c == '\n') {
        cursor_x = 0;
        if (++cursor_y >= VGA_ROWS) {
            scroll_screen();
            cursor_y = VGA_ROWS - 1;
        }
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = VGA_COLS - 1;
        }
        screen_buffer[cursor_y][cursor_x] = ((uint16_t)vga_color << 8) | ' ';
        if (sb_view == 0)
            vga_buffer[cursor_y * VGA_COLS + cursor_x] = screen_buffer[cursor_y][cursor_x];
    } else {
        screen_buffer[cursor_y][cursor_x] = ((uint16_t)vga_color << 8) | c;
        if (sb_view == 0)
            vga_buffer[cursor_y * VGA_COLS + cursor_x] = screen_buffer[cursor_y][cursor_x];
        if (++cursor_x >= VGA_COLS) {
            cursor_x = 0;
            if (++cursor_y >= VGA_ROWS) {
                scroll_screen();
                cursor_y = VGA_ROWS - 1;
            }
        }
    }
    if (sb_view == 0)
        update_cursor();
}

void console_write(const char *s) {
    while (*s)
        console_putc(*s++);
}

void console_write_dec(unsigned long value) {
    char buf[21];
    int i = 0;

    if (value == 0) {
        console_putc('0');
        return;
    }
    while (value > 0 && i < 20) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (i-- > 0)
        console_putc(buf[i]);
}

void console_write_hex8(uint8_t value) {
    static const char hex[] = "0123456789ABCDEF";
    console_putc(hex[(value >> 4) & 0xF]);
    console_putc(hex[value & 0xF]);
}

void console_write_hex16(uint16_t value) {
    console_write_hex8((value >> 8) & 0xFF);
    console_write_hex8(value & 0xFF);
}

void console_write_hex32(uint32_t value) {
    console_write_hex16((value >> 16) & 0xFFFF);
    console_write_hex16(value & 0xFFFF);
}

void console_write_hex64(uint64_t value) {
    console_write_hex32((uint32_t)(value >> 32));
    console_write_hex32((uint32_t)(value & 0xFFFFFFFF));
}

void console_init(void) {
    serial_init();
    /* Ensure VGA starts at offset zero to avoid blank screens */
    outb(0x3D4, 0x0C);
    outb(0x3D5, 0x00);
    outb(0x3D4, 0x0D);
    outb(0x3D5, 0x00);
    console_clear();
}

/* Poll keyboard driver for new input */
void console_poll_input(void) {
    static int ctrl = 0;
    char c = keyboard_read_char();
    if (!c)
        return;

    if (c == KEY_CTRL) {
        ctrl = 1;
        return;
    }

    if ((c == KEY_UP || c == KEY_DOWN) && (ctrl || sb_view)) {
        if (c == KEY_UP && sb_view < sb_count)
            sb_view++;
        else if (c == KEY_DOWN && sb_view > 0)
            sb_view--;
        render_view();
        ctrl = 0;
        return;
    }

    if (c == KEY_ALT_R) {
        if (ctrl)
            pane_move_up();
        else
            pane_move_left();
        ctrl = 0;
        return;
    }

    if (c == KEY_FN) {
        if (ctrl)
            pane_move_down();
        else
            pane_move_right();
        ctrl = 0;
        return;
    }

    switch (c) {
    case KEY_F1:
        pane_resize_width(-1);
        return;
    case KEY_F2:
        pane_resize_width(1);
        return;
    case KEY_F3:
        pane_resize_height(-1);
        return;
    case KEY_F4:
        pane_resize_height(1);
        return;
    default:
        break;
    }

    if (c == '\b' || c == 0x7F)
        c = '\b';

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

void console_get_cursor(uint8_t *x, uint8_t *y) {
    if (x)
        *x = cursor_x;
    if (y)
        *y = cursor_y;
}

void console_set_cursor(uint8_t x, uint8_t y) {
    if (x < VGA_COLS)
        cursor_x = x;
    if (y < VGA_ROWS)
        cursor_y = y;
    update_cursor();
}

void console_draw_quadrants(void) {
    for (unsigned int y = 0; y < VGA_ROWS; ++y) {
        uint16_t val = ((uint16_t)vga_color << 8) | 0xB3;
        screen_buffer[y][VGA_COLS / 2] = val;
        if (sb_view == 0)
            vga_buffer[y * VGA_COLS + VGA_COLS / 2] = val;
    }
    for (unsigned int x = 0; x < VGA_COLS; ++x) {
        uint16_t val = ((uint16_t)vga_color << 8) | 0xC4;
        screen_buffer[VGA_ROWS / 2][x] = val;
        if (sb_view == 0)
            vga_buffer[(VGA_ROWS / 2) * VGA_COLS + x] = val;
    }
    uint16_t val = ((uint16_t)vga_color << 8) | 0xC5;
    screen_buffer[VGA_ROWS / 2][VGA_COLS / 2] = val;
    if (sb_view == 0)
        vga_buffer[(VGA_ROWS / 2) * VGA_COLS + VGA_COLS / 2] = val;
    cursor_x = 1;
    cursor_y = (VGA_ROWS / 2) + 1;
    if (sb_view == 0)
        update_cursor();
    shell_show_prompt();
}
