#ifndef GUI_H
#define GUI_H
#include <stdint.h>
#include "framebuffer.h"

typedef struct Pane {
    int x;
    int y;
    int width;
    int height;
    uint32_t border_color;
} Pane;

void gui_init(void);
void gui_handle_key(char c);
void gui_task(void);
void gui_draw_pane_border(const Pane *p);

#endif /* GUI_H */
