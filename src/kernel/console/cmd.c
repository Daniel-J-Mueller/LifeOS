#include "console.h"
#include "../../lib/string.h"
#include "../inventory/inventory.h"
#include "../inventory/pci_classes.h"
#include "../kernel.h"
#include "../hmi/pane.h"

static void show_help(void) {
    console_write("Available commands:\n");
    console_write("  help  - Display this message\n");
    console_write("  clear - Clear the screen\n");
    console_write("  info  - Display hardware information\n");
    console_write("  gui start - Enable quadrant layout\n");
    console_write("  restart - Reboot the system\n");
    console_write("  shutdown - Power off the system\n");
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
    console_write("\nPCI device list:\n");
    for (unsigned int i = 0; i < inv->pci_devices && i < MAX_INVENTORY_PCI; ++i) {
        console_write("  Bus ");
        console_write_dec(inv->pci_bus[i]);
        console_write("  Vend 0x");
        console_write_hex16(inv->pci_vendor[i]);
        console_write(" Dev 0x");
        console_write_hex16(inv->pci_device[i]);
        console_write(" : ");
        console_write(pci_class_name(inv->pci_class[i]));
        console_write("\n");
    }
}

void console_execute_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        show_help();
    } else if (strcmp(cmd, "clear") == 0) {
        console_clear();
    } else if (strcmp(cmd, "info") == 0) {
        show_info();
    } else if (strcmp(cmd, "restart") == 0) {
        kernel_reboot();
    } else if (strcmp(cmd, "shutdown") == 0) {
        kernel_shutdown();
    } else if (strcmp(cmd, "exit") == 0) {
        kernel_halt();
    } else if (strcmp(cmd, "gui start") == 0) {
        pane_gui_start();
    } else if (cmd[0] != '\0') {
        console_write("Unknown command\n");
    }
}
