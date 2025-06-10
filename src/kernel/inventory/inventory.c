#include "inventory.h"
#ifndef INVENTORY_TEST
#include "../../hal/cpu.h"
#include "../../hal/memory.h"
#include "../../hal/pci.h"
#else
unsigned int hal_cpu_core_count(void);
uint64_t hal_memory_size(void);
uint32_t pci_config_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint8_t hal_pci_class_code(uint8_t bus, uint8_t dev, uint8_t func);
uint8_t hal_pci_subclass_code(uint8_t bus, uint8_t dev, uint8_t func);
#endif

/*
 * Gathers basic CPU and memory information.
 * Current implementation uses placeholders until real
 * detection code is added.
 */
static struct compute_inventory system_inventory;

void inventory_gather(void) {
    system_inventory.cpu_cores = hal_cpu_core_count();
    system_inventory.memory_bytes = hal_memory_size();
    system_inventory.pci_devices = 0;
    for (unsigned int bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t data = pci_config_read(bus, dev, func, 0);
                if ((data & 0xFFFF) != 0xFFFF) {
                    uint8_t class = hal_pci_class_code(bus, dev, func);
                    uint8_t subclass = hal_pci_subclass_code(bus, dev, func);
                    if (system_inventory.pci_devices < MAX_INVENTORY_PCI) {
                        system_inventory.pci_bus[system_inventory.pci_devices] = bus;
                        system_inventory.pci_class[system_inventory.pci_devices] = class;
                        system_inventory.pci_subclass[system_inventory.pci_devices] = subclass;
                    }
                    system_inventory.pci_devices++;
                }
            }
        }
    }
}

struct compute_inventory *inventory_get(void) {
    return &system_inventory;
}
