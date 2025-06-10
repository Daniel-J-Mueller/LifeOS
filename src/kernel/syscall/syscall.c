#include "syscall.h"
#include "../console/console.h"
#include "../sched/sched.h"

/* Initialize system call handling. Future revisions will register an
 * interrupt handler for user mode calls. */
void syscall_init(void) {
    /* Placeholder - no setup required yet */
}

/* Dispatch a system call based on its number and arguments. */
uint32_t syscall_dispatch(uint32_t num, uint32_t arg0, uint32_t arg1, uint32_t arg2) {
    switch (num) {
    case SYS_CONSOLE_WRITE:
        if (arg0)
            console_write((const char *)arg0);
        return 0;
    case SYS_YIELD:
        sched_yield();
        return 0;
    default:
        return (uint32_t)-1; /* Unknown system call */
    }
}
