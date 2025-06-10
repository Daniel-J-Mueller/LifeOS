#include "kernel.h"
#include "console/console.h"

void kernel_halt(void) {
    console_write("Halting system...\n");
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}
