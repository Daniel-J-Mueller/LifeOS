#include "kernel.h"
#include "mm/mm.h"
#include "inventory/inventory.h"
#include "console/console.h"
#include "sched/sched.h"
#include "driver/driver.h"
#include "../drivers/keyboard/keyboard.h"

/*
 * kernel_init performs early setup of core subsystems.
 * Additional initializers will be added as new components appear.
 */
void kernel_init(void) {
    mm_init();
    inventory_gather();
    console_init();
    console_write("LifeOS initialized\n");
    sched_init();

    /* Register built-in drivers */
    keyboard_register();
    driver_init_all();

    /* Other subsystem init calls will follow */
}
