#ifndef HAL_PCI_H
#define HAL_PCI_H

#include <stdint.h>
#include "io.h"

/*
 * Read a 32-bit value from PCI configuration space.
 */
static inline uint32_t pci_config_read(uint8_t bus, uint8_t dev,
                                       uint8_t func, uint8_t offset) {
    uint32_t address = (1u << 31) | ((uint32_t)bus << 16) |
                       ((uint32_t)dev << 11) | ((uint32_t)func << 8) |
                       (offset & 0xFC);
    outl(0xCF8, address);
    return inl(0xCFC);
}

/*
 * Return the class code for a device on the given bus/device/function.
 */
static inline uint8_t hal_pci_class_code(uint8_t bus, uint8_t dev,
                                         uint8_t func) {
    uint32_t data = pci_config_read(bus, dev, func, 8);
    return (data >> 24) & 0xFF;
}

/*
 * Return the subclass code for a device on the given bus/device/function.
 */
static inline uint8_t hal_pci_subclass_code(uint8_t bus, uint8_t dev,
                                            uint8_t func) {
    uint32_t data = pci_config_read(bus, dev, func, 8);
    return (data >> 16) & 0xFF;
}

/*
 * hal_pci_device_count enumerates all 256 PCI buses using the standard I/O ports
 * and returns the number of functions that respond with a valid vendor ID.
 */
static inline unsigned int hal_pci_device_count(void) {
    unsigned int count = 0;
    for (unsigned int bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t data = pci_config_read(bus, dev, func, 0);
                if ((data & 0xFFFF) != 0xFFFF) {
                    count++;
                }
            }
        }
    }
    return count;
}

#endif /* HAL_PCI_H */
