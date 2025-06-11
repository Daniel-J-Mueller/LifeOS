#include "kernel.h"
#include "mm/mm.h"
#include "inventory/inventory.h"
#include "console/console.h"
#include "sched/sched.h"
#include "syscall/syscall.h"
#include "driver/driver.h"
#include "../drivers/keyboard/keyboard.h"
#include "console/shell.h"
#include "fs/fs.h"
#include "hmi/pane.h"
#include "acpi/acpi.h"

/*
 * kernel_init performs early setup of core subsystems.
 * Additional initializers will be added as new components appear.
 */
void kernel_init(void) {
    mm_init();
    console_init();
    shell_set_active(0);
    fs_init();
    inventory_gather();
    acpi_init();
    console_write("LifeOS initialized\n");
    shell_show_prompt();
    sched_init();

    /* Register built-in drivers */
    keyboard_register();
    driver_init_all();

    syscall_init();

    /* Other subsystem init calls will follow */
}
