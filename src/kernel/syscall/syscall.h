#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* System call numbers */
enum syscall_number {
    SYS_CONSOLE_WRITE = 0,
    SYS_YIELD = 1,
    SYS_MAX
};

void syscall_init(void);
uint32_t syscall_dispatch(uint32_t num, uint32_t arg0, uint32_t arg1, uint32_t arg2);

#endif /* SYSCALL_H */
