#ifndef HAL_PCI_H
#define HAL_PCI_H

/*
 * hal_pci_device_count returns the number of PCI devices detected on
 * the primary bus. This is currently a stub that performs no real
 * enumeration and returns zero.
 */
static inline unsigned int hal_pci_device_count(void) {
    return 0;
}

#endif /* HAL_PCI_H */
