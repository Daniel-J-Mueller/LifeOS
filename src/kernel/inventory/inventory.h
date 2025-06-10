#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdint.h>

#define MAX_INVENTORY_PCI 256

struct compute_inventory {
    unsigned int cpu_cores;
    unsigned long memory_bytes;
    unsigned int pci_devices;
    uint8_t pci_class[MAX_INVENTORY_PCI];
    uint8_t pci_subclass[MAX_INVENTORY_PCI];
};

void inventory_gather(void);
struct compute_inventory *inventory_get(void);

#endif /* INVENTORY_H */
