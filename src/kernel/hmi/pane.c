#include "pane.h"
#include "../console/console.h"
#include "../console/shell.h"
#include <stdint.h>

#define VGA_COLS 80
#define VGA_ROWS 25

#define MAX_GRID 5

static int pane_enabled = 0;

/* Box drawing characters from code page 437 - double line style */
#define LINE_VERT  ((char)0xBA)
#define LINE_HORZ  ((char)0xCD)
#define LINE_CROSS ((char)0xCE)

static unsigned int grid_w = 1;
static unsigned int grid_h = 1;
static unsigned int active_x = 0;
static unsigned int active_y = 0;

void pane_init(void) {
    pane_enabled = 1;
    grid_w = grid_h = 1;
    active_x = active_y = 0;
    shell_set_active(0);
    pane_draw_no_prompt();
}

void pane_resize_width(int delta) {
    if (!pane_enabled)
        return;
    if (delta > 0 && grid_w < MAX_GRID) {
        grid_w++;
    } else if (delta < 0 && grid_w > 1) {
        grid_w--;
        if (active_x >= grid_w)
            active_x = grid_w - 1;
    }
    shell_set_active(active_y * grid_w + active_x);
    pane_draw();
}

void pane_resize_height(int delta) {
    if (!pane_enabled)
        return;
    if (delta > 0 && grid_h < MAX_GRID) {
        grid_h++;
    } else if (delta < 0 && grid_h > 1) {
        grid_h--;
        if (active_y >= grid_h)
            active_y = grid_h - 1;
    }
    shell_set_active(active_y * grid_w + active_x);
    pane_draw();
}

void pane_move_left(void) {
    if (!pane_enabled)
        return;
    if (active_x > 0)
        active_x--;
    shell_set_active(active_y * grid_w + active_x);
    pane_draw();
}

void pane_move_right(void) {
    if (!pane_enabled)
        return;
    if (active_x + 1 < grid_w)
        active_x++;
    shell_set_active(active_y * grid_w + active_x);
    pane_draw();
}

void pane_move_up(void) {
    if (!pane_enabled)
        return;
    if (active_y > 0)
        active_y--;
    shell_set_active(active_y * grid_w + active_x);
    pane_draw();
}

void pane_move_down(void) {
    if (!pane_enabled)
        return;
    if (active_y + 1 < grid_h)
        active_y++;
    shell_set_active(active_y * grid_w + active_x);
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
    console_clear();

    uint8_t old_color = console_get_color();
    console_set_color(15, 0);

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

    console_set_color(old_color & 0x0F, old_color >> 4);
    console_set_cursor(xpos[active_x] + 1, ypos[active_y] + 1);
    if (show_prompt)
        shell_show_prompt();
}

void pane_draw(void) {
    pane_draw_internal(1);
}

void pane_draw_no_prompt(void) {
    pane_draw_internal(0);
}

void pane_exit(void) {
    if (!pane_enabled)
        return;
    pane_enabled = 0;
    console_clear();
    shell_set_active(0);
    shell_show_prompt();
}

int pane_is_active(void) {
    return pane_enabled;
}

unsigned int pane_get_active_index(void) {
    return active_y * grid_w + active_x;
}

unsigned int pane_get_grid_w(void) { return grid_w; }
unsigned int pane_get_grid_h(void) { return grid_h; }
