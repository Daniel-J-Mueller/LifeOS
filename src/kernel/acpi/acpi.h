#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

/* Fixed ACPI Description Table values used by the power
 * management routines. Only the fields required for shutting
 * down the system are represented. */
struct acpi_fadt {
    uint16_t pm1a_cnt_blk; /* PM1a control register port */
    uint16_t pm1b_cnt_blk; /* PM1b control register port */
    uint16_t slp_typa;     /* SLP_TYP value for PM1a */
    uint16_t slp_typb;     /* SLP_TYP value for PM1b */
};

/* Initialize ACPI table parsing. Currently a stub that sets up
 * default FADT values for QEMU. */
void acpi_init(void);

/* Returns pointer to the FADT if discovered, otherwise NULL. */
struct acpi_fadt *acpi_get_fadt(void);

#endif /* ACPI_H */
