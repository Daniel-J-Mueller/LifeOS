#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

/* Initialize ACPI table parsing. Currently a stub that just prints
 * a message and records no tables. */
void acpi_init(void);

/* Returns pointer to the FADT if discovered, otherwise NULL. */
void *acpi_get_fadt(void);

#endif /* ACPI_H */
