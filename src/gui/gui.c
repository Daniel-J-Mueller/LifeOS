#include "gui.h"
#include "../drivers/keyboard/keyboard.h"
#include "../lib/string.h"
#include "../kernel/sched/sched.h"

static char input_buf[128];
static unsigned int input_len = 0;

static void redraw(void) {
    fb_clear(0xFFFFFFFF); // white background
    unsigned int w = fb_get_width();
    unsigned int h = fb_get_height();
    unsigned int baseline = (h * 2) / 3;
    unsigned int text_w = input_len * 8;
    int start_x = (int)(w / 2) - (int)(text_w / 2);
    fb_draw_rect(0, baseline - 8, w, 16, 0xFFFFFFFF);
    for (unsigned int i = 0; i < input_len; ++i)
        fb_draw_char(input_buf[i], start_x + i * 8, baseline - 8, 0xFF000000);
}

void gui_init(void) {
    fb_init();
    redraw();
}

void gui_handle_key(char c) {
    if (c == '\b') {
        if (input_len > 0)
            input_len--;
    } else if (c >= ' ' && input_len < sizeof(input_buf)-1) {
        input_buf[input_len++] = c;
    }
    input_buf[input_len] = '\0';
    redraw();
}

void gui_draw_pane_border(const Pane *p) {
    int x2 = p->x + p->width - 1;
    int y2 = p->y + p->height - 1;
    for (int x = p->x; x <= x2; ++x) {
        fb_draw_pixel(x, p->y, p->border_color);
        fb_draw_pixel(x, y2, p->border_color);
    }
    for (int y = p->y; y <= y2; ++y) {
        fb_draw_pixel(p->x, y, p->border_color);
        fb_draw_pixel(x2, y, p->border_color);
    }
}


void gui_task(void) {
    for (;;) {
        char c = keyboard_read_char();
        if (c)
            gui_handle_key(c);
        sched_yield();
    }
}
