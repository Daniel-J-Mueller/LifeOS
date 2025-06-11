#ifndef PANE_H
#define PANE_H

void pane_init(void);
void pane_resize_width(int delta);
void pane_resize_height(int delta);
void pane_move_left(void);
void pane_move_right(void);
void pane_move_up(void);
void pane_move_down(void);
void pane_draw(void);
void pane_draw_no_prompt(void);
void pane_store_char(char c);
void pane_clear_active_buffer(void);
void pane_begin_render(void);
void pane_end_render(void);
int pane_is_rendering(void);

#endif /* PANE_H */
