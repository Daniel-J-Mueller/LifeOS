#include "console.h"
#include "../sched/sched.h"
#include "../../drivers/keyboard/keyboard.h"
#include <stdint.h>

#define CMD_BUF_SIZE 128
#define HISTORY_LEN 8

static char history[HISTORY_LEN][CMD_BUF_SIZE];
static int history_count = 0; /* number of stored entries */
static int history_index = 0; /* next slot for new command */
static int history_nav = -1;   /* navigation offset when browsing */

static char cmd_buf[CMD_BUF_SIZE];
static unsigned int cmd_len = 0;
static unsigned int cmd_pos = 0;
static unsigned int disp_len = 0;
static uint8_t prompt_x, prompt_y;

static void history_add(const char *cmd) {
    int i = 0;
    while (cmd[i] && i < CMD_BUF_SIZE - 1) {
        history[history_index][i] = cmd[i];
        i++;
    }
    history[history_index][i] = '\0';
    history_index = (history_index + 1) % HISTORY_LEN;
    if (history_count < HISTORY_LEN)
        history_count++;
}

static const char *history_get(int offset) {
    if (offset < 0 || offset >= history_count)
        return "";
    int idx = history_index - 1 - offset;
    if (idx < 0)
        idx += HISTORY_LEN;
    return history[idx];
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
    cmd_len = cmd_pos = disp_len = 0;
    console_write("LifeOS> ");
    console_get_cursor(&prompt_x, &prompt_y);
}

void shell_task(void) {
    shell_show_prompt();

    for (;;) {
        char c = console_getc();
        if (c) {
            if (c == '\n') {
                cmd_buf[cmd_len] = '\0';
                console_set_cursor(prompt_x + disp_len, prompt_y);
                console_putc('\n');
                if (cmd_len > 0)
                    history_add(cmd_buf);
                history_nav = -1;
                console_execute_command(cmd_buf);
                shell_show_prompt();
            } else if (c == '\b') {
                if (cmd_pos > 0) {
                    for (unsigned int i = cmd_pos - 1; i < cmd_len - 1; ++i)
                        cmd_buf[i] = cmd_buf[i + 1];
                    cmd_len--; cmd_pos--;
                    redraw_line(cmd_buf, cmd_len, cmd_pos, prompt_x, prompt_y, &disp_len);
                }
            } else if (c == KEY_LEFT) {
                if (cmd_pos > 0) {
                    cmd_pos--;
                    console_set_cursor(prompt_x + cmd_pos, prompt_y);
                }
            } else if (c == KEY_RIGHT) {
                if (cmd_pos < cmd_len) {
                    cmd_pos++;
                    console_set_cursor(prompt_x + cmd_pos, prompt_y);
                }
            } else if (c == KEY_UP) {
                if (history_count > 0 && history_nav < history_count - 1) {
                    history_nav++;
                    const char *h = history_get(history_nav);
                    for (cmd_len = 0; h[cmd_len] && cmd_len < CMD_BUF_SIZE - 1; ++cmd_len)
                        cmd_buf[cmd_len] = h[cmd_len];
                    cmd_pos = cmd_len;
                    redraw_line(cmd_buf, cmd_len, cmd_pos, prompt_x, prompt_y, &disp_len);
                }
            } else if (c == KEY_DOWN) {
                if (history_nav >= 0) {
                    history_nav--;
                    const char *h = (history_nav >= 0) ? history_get(history_nav) : "";
                    for (cmd_len = 0; h[cmd_len] && cmd_len < CMD_BUF_SIZE - 1; ++cmd_len)
                        cmd_buf[cmd_len] = h[cmd_len];
                    cmd_pos = cmd_len;
                    redraw_line(cmd_buf, cmd_len, cmd_pos, prompt_x, prompt_y, &disp_len);
                }
            } else {
                if (cmd_len < CMD_BUF_SIZE - 1) {
                    for (unsigned int i = cmd_len; i > cmd_pos; --i)
                        cmd_buf[i] = cmd_buf[i - 1];
                    cmd_buf[cmd_pos++] = c;
                    cmd_len++;
                    redraw_line(cmd_buf, cmd_len, cmd_pos, prompt_x, prompt_y, &disp_len);
                }
                history_nav = -1;
            }
        }
        sched_yield();
    }
}
