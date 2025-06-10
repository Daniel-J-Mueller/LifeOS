#include "kernel.h"
#include "console/console.h"
#include "../hal/io.h"

void kernel_halt(void) {
    console_write("Halting system...\n");
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}

void kernel_reboot(void) {
    console_write("Rebooting system...\n");
    /* Try keyboard controller reboot first */
    for (;;) {
        __asm__ volatile("cli");
        outb(0x64, 0xFE);
        __asm__ volatile("hlt");
    }
}
