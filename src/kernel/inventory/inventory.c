#include "inventory.h"
#include "../../hal/cpu.h"
#include "../../hal/memory.h"

/*
 * Gathers basic CPU and memory information.
 * Current implementation uses placeholders until real
 * detection code is added.
 */
static struct compute_inventory system_inventory;

void inventory_gather(void) {
    system_inventory.cpu_cores = hal_cpu_core_count();
    system_inventory.memory_bytes = hal_memory_size();
}

struct compute_inventory *inventory_get(void) {
    return &system_inventory;
}
