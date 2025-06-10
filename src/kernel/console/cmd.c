#include "console.h"
#include "../../lib/string.h"

static void show_help(void) {
    console_write("Available commands:\n");
    console_write("  help  - Display this message\n");
    console_write("  clear - Clear the screen\n");
}

void console_execute_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        show_help();
    } else if (strcmp(cmd, "clear") == 0) {
        console_clear();
    } else if (cmd[0] != '\0') {
        console_write("Unknown command\n");
    }
}
