#include <assert.h>
#include <stdint.h>
#include "../src/kernel/inventory/inventory.h"

/* Stub HAL implementations */
unsigned int hal_cpu_core_count(void) { return 4; }
uint64_t hal_memory_size(void) { return 128ULL * 1024 * 1024; }

static int pci_device_lookup(uint8_t bus, uint8_t dev, uint8_t func) {
    return (bus == 0 && dev == 0 && func < 2) ||
           (bus == 1 && dev == 0 && func == 0);
}

uint32_t pci_config_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    (void)offset;
    return pci_device_lookup(bus, dev, func) ? 0x12345678 : 0xFFFF;
}

uint8_t hal_pci_class_code(uint8_t bus, uint8_t dev, uint8_t func) {
    (void)bus; (void)dev; (void)func;
    return 0x02;
}

uint8_t hal_pci_subclass_code(uint8_t bus, uint8_t dev, uint8_t func) {
    (void)bus; (void)dev; (void)func;
    return 0x00;
}

int main(void) {
    inventory_gather();
    struct compute_inventory *inv = inventory_get();
    assert(inv->cpu_cores == 4);
    assert(inv->memory_bytes == 128ULL * 1024 * 1024);
    assert(inv->pci_devices == 3);
    for (unsigned int i = 0; i < inv->pci_devices; ++i) {
        assert(inv->pci_class[i] == 0x02);
        assert(inv->pci_subclass[i] == 0x00);
    }
    return 0;
}
