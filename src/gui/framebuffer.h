#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <stdint.h>

#define FRAMEBUFFER_ADDR 0xE0000000
#define FRAMEBUFFER_WIDTH 1024
#define FRAMEBUFFER_HEIGHT 768
#define FRAMEBUFFER_PITCH (FRAMEBUFFER_WIDTH * 4)

void fb_init(void);
void fb_clear(uint32_t color);
void fb_draw_pixel(int x, int y, uint32_t color);
void fb_draw_rect(int x, int y, int w, int h, uint32_t color);
void fb_draw_char(char c, int x, int y, uint32_t color);
void fb_draw_text(const char *str, int x, int y, uint32_t color);
unsigned int fb_get_width(void);
unsigned int fb_get_height(void);

#endif /* FRAMEBUFFER_H */
