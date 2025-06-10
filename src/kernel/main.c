#include <stdint.h>

/*
 * Primary C entry point for the kernel. The environment is assumed
 * to be in 32-bit protected mode with a flat memory model and a
 * temporary stack set up by the assembly entry stub.
 */
void kmain(void) {
    /* Placeholder for future initialization code. */
    for (;;) {
        __asm__("hlt");
    }
}
