#include <assert.h>
#include <stdint.h>
#include "../src/kernel/inventory/inventory.h"

unsigned int hal_cpu_core_count(void) { return 8; }
uint64_t hal_memory_size(void) { return 256ULL * 1024 * 1024; }

static int gpu_present(uint8_t bus, uint8_t dev, uint8_t func) {
    return (func == 0 && dev == 0 && (bus == 1 || bus == 2));
}

uint32_t pci_config_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    (void)offset;
    return gpu_present(bus, dev, func) ? 0xDEADBEEF : 0xFFFF;
}

uint16_t hal_pci_vendor_id(uint8_t bus, uint8_t dev, uint8_t func) {
    (void)bus; (void)dev; (void)func;
    return 0xBEEF;
}

uint16_t hal_pci_device_id(uint8_t bus, uint8_t dev, uint8_t func) {
    (void)bus; (void)dev; (void)func;
    return 0xFEED;
}

uint8_t hal_pci_class_code(uint8_t bus, uint8_t dev, uint8_t func) {
    return gpu_present(bus, dev, func) ? 0x03 : 0x00;
}

uint8_t hal_pci_subclass_code(uint8_t bus, uint8_t dev, uint8_t func) {
    return 0x00;
}

int main(void) {
    inventory_gather();
    struct compute_inventory *inv = inventory_get();
    assert(inv->pci_devices == 2);
    assert(inv->pci_bus[0] == 1);
    assert(inv->pci_bus[1] == 2);
    for (int i = 0; i < 2; ++i) {
        assert(inv->pci_class[i] == 0x03);
        assert(inv->pci_vendor[i] == 0xBEEF);
        assert(inv->pci_device[i] == 0xFEED);
    }
    return 0;
}
