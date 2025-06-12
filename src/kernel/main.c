#include <stdint.h>
#include "kernel.h"
#include "console/console.h"
#include "sched/sched.h"
#include "console/shell.h"
#include "syscall/syscall.h"
#include "../gui/gui.h"

static void idle_task(void) {
    for (;;) {
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
    struct task *shell = sched_create_task(shell_task);
    sched_add_task(shell);
    struct task *gui = sched_create_task(gui_task);
    sched_add_task(gui);

    sched_start();
}
