#ifndef INVENTORY_H
#define INVENTORY_H

struct compute_inventory {
    unsigned int cpu_cores;
    unsigned long memory_bytes;
    unsigned int pci_devices;
};

void inventory_gather(void);
struct compute_inventory *inventory_get(void);

#endif /* INVENTORY_H */
