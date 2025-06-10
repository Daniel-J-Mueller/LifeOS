#include <stdint.h>
#include "kernel.h"
#include "console/console.h"
#include "sched/sched.h"
#include "console/shell.h"
#include "syscall/syscall.h"

static void idle_task(void) {
    for (;;) {
        console_poll_input();
        __asm__("hlt");
        sched_yield();
    }
}

static void demo_task(void) {
    for (;;) {
        syscall_dispatch(SYS_CONSOLE_WRITE, (uint32_t)"Demo task running\n", 0, 0);
        sched_yield();
    }
}

/*
 * Primary C entry point for the kernel. The environment is assumed
 * to be in 32-bit protected mode with a flat memory model and a
 * temporary stack set up by the assembly entry stub.
 */
void kmain(void) {
    kernel_init();
    struct task *idle = sched_create_task(idle_task);
    sched_add_task(idle);

    struct task *demo = sched_create_task(demo_task);
    sched_add_task(demo);

    struct task *shell = sched_create_task(shell_task);
    sched_add_task(shell);

    sched_start();
}
