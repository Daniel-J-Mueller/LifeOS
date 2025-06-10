#include "console.h"
#include "../../lib/string.h"
#include "../inventory/inventory.h"
#include "../kernel.h"

static void show_help(void) {
    console_write("Available commands:\n");
    console_write("  help  - Display this message\n");
    console_write("  clear - Clear the screen\n");
    console_write("  info  - Display hardware information\n");
    console_write("  exit  - Halt the system\n");
}

static void show_info(void) {
    struct compute_inventory *inv = inventory_get();
    console_write("CPU cores: ");
    console_write_dec(inv->cpu_cores);
    console_write("\nMemory bytes: ");
    console_write_dec(inv->memory_bytes);
    console_write("\nPCI devices: ");
    console_write_dec(inv->pci_devices);
    console_write("\n");
}

void console_execute_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        show_help();
    } else if (strcmp(cmd, "clear") == 0) {
        console_clear();
    } else if (strcmp(cmd, "info") == 0) {
        show_info();
    } else if (strcmp(cmd, "exit") == 0) {
        kernel_halt();
    } else if (cmd[0] != '\0') {
        console_write("Unknown command\n");
    }
}
