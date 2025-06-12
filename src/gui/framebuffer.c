#include "framebuffer.h"
#include "font8x8_basic.h"

static uint32_t *fb = (uint32_t *)FRAMEBUFFER_ADDR;
static unsigned int fb_width = FRAMEBUFFER_WIDTH;
static unsigned int fb_height = FRAMEBUFFER_HEIGHT;
static unsigned int fb_pitch = FRAMEBUFFER_PITCH;

void fb_init(void) {
    fb = (uint32_t *)FRAMEBUFFER_ADDR;
    fb_width = FRAMEBUFFER_WIDTH;
    fb_height = FRAMEBUFFER_HEIGHT;
    fb_pitch = FRAMEBUFFER_PITCH;
}

unsigned int fb_get_width(void) { return fb_width; }
unsigned int fb_get_height(void) { return fb_height; }

void fb_clear(uint32_t color) {
    for (unsigned int y = 0; y < fb_height; ++y) {
        for (unsigned int x = 0; x < fb_width; ++x) {
            fb[y * (fb_pitch/4) + x] = color;
        }
    }
}

void fb_draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= (int)fb_width || y >= (int)fb_height)
        return;
    fb[y * (fb_pitch/4) + x] = color;
}

void fb_draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int j = 0; j < h; ++j)
        for (int i = 0; i < w; ++i)
            fb_draw_pixel(x + i, y + j, color);
}

void fb_draw_char(char c, int x, int y, uint32_t color) {
    const unsigned char *glyph = (const unsigned char *)font8x8_basic[(unsigned char)c];
    for (int row = 0; row < 8; ++row) {
        unsigned char bits = glyph[row];
        for (int col = 0; col < 8; ++col) {
            if (bits & (1 << col))
                fb_draw_pixel(x + col, y + row, color);
        }
    }
}

void fb_draw_text(const char *str, int x, int y, uint32_t color) {
    while (*str) {
        fb_draw_char(*str, x, y, color);
        x += 8;
        str++;
    }
}

