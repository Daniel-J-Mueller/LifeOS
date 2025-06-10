#include "kernel.h"
#include "mm/mm.h"

/*
 * kernel_init performs early setup of core subsystems.
 * Additional initializers will be added as new components appear.
 */
void kernel_init(void) {
    mm_init();
    /* Other subsystem init calls will follow */
}
