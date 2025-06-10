#include "console.h"
#include "../sched/sched.h"

#define CMD_BUF_SIZE 128

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
                console_execute_command(cmd_buf);
                idx = 0;
                console_write("LifeOS> ");
            } else if (c == '\b') {
                if (idx > 0) {
                    idx--;
                    console_putc('\b');
                }
            } else {
                if (idx < CMD_BUF_SIZE - 1) {
                    cmd_buf[idx++] = c;
                    console_putc(c);
                }
            }
        }
        sched_yield();
    }
}
