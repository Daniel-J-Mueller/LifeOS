#include "inventory.h"
#include "../../hal/cpu.h"
#include "../../hal/memory.h"
#include "../../hal/pci.h"

/*
 * Gathers basic CPU and memory information.
 * Current implementation uses placeholders until real
 * detection code is added.
 */
static struct compute_inventory system_inventory;

void inventory_gather(void) {
    system_inventory.cpu_cores = hal_cpu_core_count();
    system_inventory.memory_bytes = hal_memory_size();
    system_inventory.pci_devices = hal_pci_device_count();
}

struct compute_inventory *inventory_get(void) {
    return &system_inventory;
}
