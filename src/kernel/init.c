#include "kernel.h"
#include "mm/mm.h"
#include "inventory/inventory.h"
#include "console/console.h"

/*
 * kernel_init performs early setup of core subsystems.
 * Additional initializers will be added as new components appear.
 */
void kernel_init(void) {
    mm_init();
    inventory_gather();
    console_init();
    console_write("LifeOS initialized\n");
    /* Other subsystem init calls will follow */
}
