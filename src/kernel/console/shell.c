#include "console.h"
#include "../sched/sched.h"
#include "../../drivers/keyboard/keyboard.h"

#define CMD_BUF_SIZE 128
#define HISTORY_LEN 8

static char history[HISTORY_LEN][CMD_BUF_SIZE];
static int history_count = 0; /* number of stored entries */
static int history_index = 0; /* next slot for new command */
static int history_nav = -1;   /* navigation offset when browsing */

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

void shell_task(void) {
    char cmd_buf[CMD_BUF_SIZE];
    unsigned int len = 0;
    unsigned int pos = 0;
    unsigned int disp_len = 0;
    uint8_t prompt_x, prompt_y;

    console_write("LifeOS> ");
    console_get_cursor(&prompt_x, &prompt_y);

    for (;;) {
        char c = console_getc();
        if (c) {
            if (c == '\n') {
                cmd_buf[len] = '\0';
                console_set_cursor(prompt_x + disp_len, prompt_y);
                console_putc('\n');
                if (len > 0)
                    history_add(cmd_buf);
                history_nav = -1;
                console_execute_command(cmd_buf);
                len = pos = disp_len = 0;
                console_write("LifeOS> ");
                console_get_cursor(&prompt_x, &prompt_y);
            } else if (c == '\b') {
                if (pos > 0) {
                    for (unsigned int i = pos - 1; i < len - 1; ++i)
                        cmd_buf[i] = cmd_buf[i + 1];
                    len--; pos--;
                    redraw_line(cmd_buf, len, pos, prompt_x, prompt_y, &disp_len);
                }
            } else if (c == KEY_LEFT) {
                if (pos > 0) {
                    pos--;
                    console_set_cursor(prompt_x + pos, prompt_y);
                }
            } else if (c == KEY_RIGHT) {
                if (pos < len) {
                    pos++;
                    console_set_cursor(prompt_x + pos, prompt_y);
                }
            } else if (c == KEY_UP) {
                if (history_count > 0 && history_nav < history_count - 1) {
                    history_nav++;
                    const char *h = history_get(history_nav);
                    for (len = 0; h[len] && len < CMD_BUF_SIZE - 1; ++len)
                        cmd_buf[len] = h[len];
                    pos = len;
                    redraw_line(cmd_buf, len, pos, prompt_x, prompt_y, &disp_len);
                }
            } else if (c == KEY_DOWN) {
                if (history_nav >= 0) {
                    history_nav--;
                    const char *h = (history_nav >= 0) ? history_get(history_nav) : "";
                    for (len = 0; h[len] && len < CMD_BUF_SIZE - 1; ++len)
                        cmd_buf[len] = h[len];
                    pos = len;
                    redraw_line(cmd_buf, len, pos, prompt_x, prompt_y, &disp_len);
                }
            } else {
                if (len < CMD_BUF_SIZE - 1) {
                    for (unsigned int i = len; i > pos; --i)
                        cmd_buf[i] = cmd_buf[i - 1];
                    cmd_buf[pos++] = c;
                    len++;
                    redraw_line(cmd_buf, len, pos, prompt_x, prompt_y, &disp_len);
                }
                history_nav = -1;
            }
        }
        sched_yield();
    }
}
