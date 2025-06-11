#include "console.h"
#include "../sched/sched.h"
#include "../../drivers/keyboard/keyboard.h"
#include <stdint.h>

#define CMD_BUF_SIZE 128
#define HISTORY_LEN 8
#define MAX_PANES 25

struct shell_context {
    char history[HISTORY_LEN][CMD_BUF_SIZE];
    int history_count;
    int history_index;
    int history_nav;
    char cmd_buf[CMD_BUF_SIZE];
    unsigned int cmd_len;
    unsigned int cmd_pos;
    unsigned int disp_len;
    uint8_t prompt_x, prompt_y;
};

static struct shell_context shells[MAX_PANES];
static struct shell_context *cur = &shells[0];

void shell_set_active(int index) {
    if (index >= 0 && index < MAX_PANES)
        cur = &shells[index];
}

static void history_add(const char *cmd) {
    int i = 0;
    while (cmd[i] && i < CMD_BUF_SIZE - 1) {
        cur->history[cur->history_index][i] = cmd[i];
        i++;
    }
    cur->history[cur->history_index][i] = '\0';
    cur->history_index = (cur->history_index + 1) % HISTORY_LEN;
    if (cur->history_count < HISTORY_LEN)
        cur->history_count++;
}

static const char *history_get(int offset) {
    if (offset < 0 || offset >= cur->history_count)
        return "";
    int idx = cur->history_index - 1 - offset;
    if (idx < 0)
        idx += HISTORY_LEN;
    return cur->history[idx];
}

static void redraw_line(const char *buf, unsigned int len, unsigned int pos,
                        uint8_t start_x, uint8_t start_y,
                        unsigned int *disp_len) {
    console_set_cursor(start_x, start_y);
    for (unsigned int i = 0; i < *disp_len; ++i)
        console_putc(' ');
    console_set_cursor(start_x, start_y);
    for (unsigned int i = 0; i < len; ++i)
        console_putc(buf[i]);
    *disp_len = len;
    console_set_cursor(start_x + pos, start_y);
}

void shell_show_prompt(void) {
    cur->cmd_len = cur->cmd_pos = cur->disp_len = 0;
    console_write("LifeOS> ");
    console_get_cursor(&cur->prompt_x, &cur->prompt_y);
}

void shell_task(void) {

    for (;;) {
        char c = console_getc();
        if (c) {
            if (c == '\n') {
                cur->cmd_buf[cur->cmd_len] = '\0';
                console_set_cursor(cur->prompt_x + cur->disp_len, cur->prompt_y);
                console_putc('\n');
                if (cur->cmd_len > 0)
                    history_add(cur->cmd_buf);
                cur->history_nav = -1;
                console_execute_command(cur->cmd_buf);
                shell_show_prompt();
            } else if (c == '\b') {
                if (cur->cmd_pos > 0) {
                    for (unsigned int i = cur->cmd_pos - 1; i < cur->cmd_len - 1; ++i)
                        cur->cmd_buf[i] = cur->cmd_buf[i + 1];
                    cur->cmd_len--; cur->cmd_pos--;
                    redraw_line(cur->cmd_buf, cur->cmd_len, cur->cmd_pos, cur->prompt_x, cur->prompt_y, &cur->disp_len);
                }
            } else if (c == KEY_LEFT) {
                if (cur->cmd_pos > 0) {
                    cur->cmd_pos--;
                    console_set_cursor(cur->prompt_x + cur->cmd_pos, cur->prompt_y);
                }
            } else if (c == KEY_RIGHT) {
                if (cur->cmd_pos < cur->cmd_len) {
                    cur->cmd_pos++;
                    console_set_cursor(cur->prompt_x + cur->cmd_pos, cur->prompt_y);
                }
            } else if (c == KEY_UP) {
                if (cur->history_count > 0 && cur->history_nav < cur->history_count - 1) {
                    cur->history_nav++;
                    const char *h = history_get(cur->history_nav);
                    for (cur->cmd_len = 0; h[cur->cmd_len] && cur->cmd_len < CMD_BUF_SIZE - 1; ++cur->cmd_len)
                        cur->cmd_buf[cur->cmd_len] = h[cur->cmd_len];
                    cur->cmd_pos = cur->cmd_len;
                    redraw_line(cur->cmd_buf, cur->cmd_len, cur->cmd_pos, cur->prompt_x, cur->prompt_y, &cur->disp_len);
                }
            } else if (c == KEY_DOWN) {
                if (cur->history_nav >= 0) {
                    cur->history_nav--;
                    const char *h = (cur->history_nav >= 0) ? history_get(cur->history_nav) : "";
                    for (cur->cmd_len = 0; h[cur->cmd_len] && cur->cmd_len < CMD_BUF_SIZE - 1; ++cur->cmd_len)
                        cur->cmd_buf[cur->cmd_len] = h[cur->cmd_len];
                    cur->cmd_pos = cur->cmd_len;
                    redraw_line(cur->cmd_buf, cur->cmd_len, cur->cmd_pos, cur->prompt_x, cur->prompt_y, &cur->disp_len);
                }
            } else {
                if (cur->cmd_len < CMD_BUF_SIZE - 1) {
                    for (unsigned int i = cur->cmd_len; i > cur->cmd_pos; --i)
                        cur->cmd_buf[i] = cur->cmd_buf[i - 1];
                    cur->cmd_buf[cur->cmd_pos++] = c;
                    cur->cmd_len++;
                    redraw_line(cur->cmd_buf, cur->cmd_len, cur->cmd_pos, cur->prompt_x, cur->prompt_y, &cur->disp_len);
                }
                cur->history_nav = -1;
            }
        }
        sched_yield();
    }
}
