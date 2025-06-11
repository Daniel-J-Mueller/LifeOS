#include "pane.h"
#include "../console/console.h"
#include "../console/shell.h"
#include <stdint.h>

#define VGA_COLS 80
#define VGA_ROWS 25

#define MAX_GRID 4

/* Box drawing characters from code page 437 */
#define LINE_VERT  ((char)0xB3)
#define LINE_HORZ  ((char)0xC4)
#define LINE_CROSS ((char)0xC5)

static unsigned int grid_w = 1;
static unsigned int grid_h = 1;
static unsigned int active_x = 0;
static unsigned int active_y = 0;

static char pane_buf[MAX_GRID][MAX_GRID][VGA_ROWS][VGA_COLS];
static uint8_t cursor_x[MAX_GRID][MAX_GRID];
static uint8_t cursor_y[MAX_GRID][MAX_GRID];
static int rendering = 0;

void pane_init(void) {
    grid_w = grid_h = 1;
    active_x = active_y = 0;
    for (unsigned int y = 0; y < MAX_GRID; ++y)
        for (unsigned int x = 0; x < MAX_GRID; ++x) {
            for (unsigned int r = 0; r < VGA_ROWS; ++r)
                for (unsigned int c = 0; c < VGA_COLS; ++c)
                    pane_buf[y][x][r][c] = ' ';
            cursor_x[y][x] = cursor_y[y][x] = 0;
        }
    pane_draw_no_prompt();
}

void pane_resize_width(int delta) {
    if (delta > 0 && grid_w < MAX_GRID) {
        grid_w++;
    } else if (delta < 0 && grid_w > 1) {
        grid_w--;
        if (active_x >= grid_w)
            active_x = grid_w - 1;
    }
    pane_draw();
}

void pane_resize_height(int delta) {
    if (delta > 0 && grid_h < MAX_GRID) {
        grid_h++;
    } else if (delta < 0 && grid_h > 1) {
        grid_h--;
        if (active_y >= grid_h)
            active_y = grid_h - 1;
    }
    pane_draw();
}

void pane_move_left(void) {
    if (active_x > 0)
        active_x--;
    pane_draw();
}

void pane_move_right(void) {
    if (active_x + 1 < grid_w)
        active_x++;
    pane_draw();
}

void pane_move_up(void) {
    if (active_y > 0)
        active_y--;
    pane_draw();
}

void pane_move_down(void) {
    if (active_y + 1 < grid_h)
        active_y++;
    pane_draw();
}

static void compute_layout(unsigned int widths[], unsigned int heights[]) {
    unsigned int remaining_w = VGA_COLS;
    for (unsigned int i = 0; i < grid_w; ++i) {
        if (i == active_x && grid_w > 1) {
            widths[i] = VGA_COLS / 2;
            remaining_w -= widths[i];
        }
    }
    unsigned int normal_w = (grid_w > 1) ? remaining_w / (grid_w - 1) : VGA_COLS;
    for (unsigned int i = 0; i < grid_w; ++i) {
        if (i != active_x)
            widths[i] = normal_w;
    }
    unsigned int sum = 0;
    for (unsigned int i = 0; i < grid_w; ++i)
        sum += widths[i];
    widths[grid_w - 1] += VGA_COLS - sum;

    unsigned int remaining_h = VGA_ROWS;
    for (unsigned int j = 0; j < grid_h; ++j) {
        if (j == active_y && grid_h > 1) {
            heights[j] = VGA_ROWS / 2;
            remaining_h -= heights[j];
        }
    }
    unsigned int normal_h = (grid_h > 1) ? remaining_h / (grid_h - 1) : VGA_ROWS;
    for (unsigned int j = 0; j < grid_h; ++j) {
        if (j != active_y)
            heights[j] = normal_h;
    }
    sum = 0;
    for (unsigned int j = 0; j < grid_h; ++j)
        sum += heights[j];
    heights[grid_h - 1] += VGA_ROWS - sum;
}

static void pane_draw_internal(int show_prompt) {
    pane_begin_render();
    console_clear();

    unsigned int widths[MAX_GRID];
    unsigned int heights[MAX_GRID];
    compute_layout(widths, heights);

    unsigned int xpos[MAX_GRID];
    unsigned int ypos[MAX_GRID];
    xpos[0] = 0;
    for (unsigned int i = 1; i < grid_w; ++i)
        xpos[i] = xpos[i - 1] + widths[i - 1];
    ypos[0] = 0;
    for (unsigned int j = 1; j < grid_h; ++j)
        ypos[j] = ypos[j - 1] + heights[j - 1];

    for (unsigned int py = 0; py < grid_h; ++py)
        for (unsigned int px = 0; px < grid_w; ++px)
            for (unsigned int sy = 0; sy < heights[py]; ++sy) {
                unsigned int src_y = (sy * VGA_ROWS) / heights[py];
                for (unsigned int sx = 0; sx < widths[px]; ++sx) {
                    unsigned int src_x = (sx * VGA_COLS) / widths[px];
                    console_set_cursor(xpos[px] + sx, ypos[py] + sy);
                    console_putc(pane_buf[py][px][src_y][src_x]);
                }
            }

    for (unsigned int i = 1; i < grid_w; ++i) {
        unsigned int x = xpos[i];
        for (unsigned int y = 0; y < VGA_ROWS; ++y) {
            console_set_cursor(x, y);
            console_putc(LINE_VERT);
        }
    }

    for (unsigned int j = 1; j < grid_h; ++j) {
        unsigned int y = ypos[j];
        console_set_cursor(0, y);
        for (unsigned int x = 0; x < VGA_COLS; ++x)
            console_putc(LINE_HORZ);
    }

    for (unsigned int i = 1; i < grid_w; ++i)
        for (unsigned int j = 1; j < grid_h; ++j) {
            console_set_cursor(xpos[i], ypos[j]);
            console_putc(LINE_CROSS);
        }

    console_set_cursor(xpos[active_x] + cursor_x[active_y][active_x],
                       ypos[active_y] + cursor_y[active_y][active_x]);
    pane_end_render();
    if (show_prompt)
        shell_show_prompt();
}

void pane_draw(void) {
    pane_draw_internal(1);
}

void pane_draw_no_prompt(void) {
    pane_draw_internal(0);
}

void pane_store_char(char c) {
    unsigned int px = active_x;
    unsigned int py = active_y;
    uint8_t x = cursor_x[py][px];
    uint8_t y = cursor_y[py][px];

    if (c == '\n') {
        x = 0;
        if (++y >= VGA_ROWS)
            y = 0;
    } else if (c == '\b') {
        if (x > 0) {
            x--;
        } else if (y > 0) {
            y--;
            x = VGA_COLS - 1;
        }
        pane_buf[py][px][y][x] = ' ';
    } else {
        pane_buf[py][px][y][x] = c;
        if (++x >= VGA_COLS) {
            x = 0;
            if (++y >= VGA_ROWS)
                y = 0;
        }
    }

    cursor_x[py][px] = x;
    cursor_y[py][px] = y;
}

void pane_clear_active_buffer(void) {
    unsigned int px = active_x;
    unsigned int py = active_y;
    for (unsigned int r = 0; r < VGA_ROWS; ++r)
        for (unsigned int c = 0; c < VGA_COLS; ++c)
            pane_buf[py][px][r][c] = ' ';
    cursor_x[py][px] = cursor_y[py][px] = 0;
}

void pane_begin_render(void) { rendering = 1; }
void pane_end_render(void) { rendering = 0; }
int pane_is_rendering(void) { return rendering; }
