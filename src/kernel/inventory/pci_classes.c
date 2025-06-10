#include "pci_classes.h"

struct class_map { uint8_t code; const char *name; };

static const struct class_map pci_class_map[] = {
    {0x00, "Unclassified"},
    {0x01, "Mass Storage"},
    {0x02, "Network"},
    {0x03, "Display"},
    {0x04, "Multimedia"},
    {0x05, "Memory"},
    {0x06, "Bridge"},
    {0x07, "Communication"},
    {0x08, "System"},
    {0x09, "Input"},
    {0x0A, "Docking"},
    {0x0B, "Processor"},
    {0x0C, "Serial Bus"},
    {0x0D, "Wireless"},
    {0x0E, "Intelligent I/O"},
    {0x0F, "Satellite"},
    {0x10, "Encryption"},
    {0x11, "Signal Processing"},
    {0x12, "Processing Accel"},
    {0x13, "Instrumentation"}
};

const char *pci_class_name(uint8_t class_code) {
    for (unsigned int i = 0; i < sizeof(pci_class_map)/sizeof(pci_class_map[0]); ++i) {
        if (pci_class_map[i].code == class_code)
            return pci_class_map[i].name;
    }
    return "Unknown";
}
