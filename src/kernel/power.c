#include "kernel.h"
#include "console/console.h"
#include "../hal/io.h"
#include "acpi/acpi.h"

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

void kernel_shutdown(void) {
    console_write("Shutting down...\n");
    struct acpi_fadt *fadt = acpi_get_fadt();
    uint16_t pm1a = 0x604;
    uint16_t pm1b = 0xB004;
    uint16_t val_a = 0x2000;
    uint16_t val_b = 0x2000;

    if (fadt) {
        if (fadt->pm1a_cnt_blk)
            pm1a = fadt->pm1a_cnt_blk;
        if (fadt->pm1b_cnt_blk)
            pm1b = fadt->pm1b_cnt_blk;
        val_a = fadt->slp_typa | 0x2000;
        val_b = fadt->slp_typb | 0x2000;
    }

    for (;;) {
        __asm__ volatile("cli");
        if (pm1a)
            outw(pm1a, val_a);
        if (pm1b)
            outw(pm1b, val_b);
        __asm__ volatile("hlt");
    }
}
