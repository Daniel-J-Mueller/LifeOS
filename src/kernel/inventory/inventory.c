#include "inventory.h"

/*
 * Gathers basic CPU and memory information.
 * Current implementation uses placeholders until real
 * detection code is added.
 */
static struct compute_inventory system_inventory;

void inventory_gather(void) {
    system_inventory.cpu_cores = 1; /* TODO: detect actual CPU cores */
    system_inventory.memory_bytes = 64UL * 1024 * 1024; /* TODO: detect RAM */
}

struct compute_inventory *inventory_get(void) {
    return &system_inventory;
}
