#include <stdint.h>
#include "kernel.h"

/*
 * Primary C entry point for the kernel. The environment is assumed
 * to be in 32-bit protected mode with a flat memory model and a
 * temporary stack set up by the assembly entry stub.
 */
void kmain(void) {
    kernel_init();
    for (;;) {
        __asm__("hlt");
    }
}
