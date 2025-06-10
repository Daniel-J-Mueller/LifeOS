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

void shell_task(void) {
    char cmd_buf[CMD_BUF_SIZE];
    unsigned int idx = 0;

    console_write("LifeOS> ");
    for (;;) {
        char c = console_getc();
        if (c) {
            if (c == '\n') {
                cmd_buf[idx] = '\0';
                console_putc('\n');
                if (idx > 0)
                    history_add(cmd_buf);
                history_nav = -1;
                console_execute_command(cmd_buf);
                idx = 0;
                console_write("LifeOS> ");
            } else if (c == '\b') {
                if (idx > 0) {
                    idx--;
                    console_putc('\b');
                }
            } else if (c == KEY_UP) {
                if (history_count > 0 && history_nav < history_count - 1) {
                    history_nav++;
                    const char *h = history_get(history_nav);
                    while (idx > 0) {
                        idx--;
                        console_putc('\b');
                    }
                    unsigned int i = 0;
                    while (h[i] && i < CMD_BUF_SIZE - 1) {
                        cmd_buf[i] = h[i];
                        console_putc(h[i]);
                        i++;
                    }
                    idx = i;
                }
            } else if (c == KEY_DOWN) {
                if (history_nav >= 0) {
                    history_nav--;
                    const char *h = (history_nav >= 0) ? history_get(history_nav) : "";
                    while (idx > 0) {
                        idx--;
                        console_putc('\b');
                    }
                    unsigned int i = 0;
                    while (h[i] && i < CMD_BUF_SIZE - 1) {
                        cmd_buf[i] = h[i];
                        console_putc(h[i]);
                        i++;
                    }
                    idx = i;
                }
            } else {
                if (idx < CMD_BUF_SIZE - 1) {
                    cmd_buf[idx++] = c;
                    console_putc(c);
                }
                history_nav = -1;
            }
        }
        sched_yield();
    }
}
