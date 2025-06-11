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

/* Standard ACPI System Description Table header. */
struct acpi_sdt_header {
    char     signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

/* Initialize ACPI table parsing. The routine searches the EBDA and BIOS
 * areas for the Root System Description Pointer. Both RSDT and XSDT
 * layouts are supported so the parser can operate on real firmware
 * tables. If discovery fails, default values targeting QEMU are used. */
void acpi_init(void);

/* Returns pointer to the FADT if discovered, otherwise NULL. */
struct acpi_fadt *acpi_get_fadt(void);

/* Search the parsed table list for the given signature and return
 * a pointer to the matching header or NULL if not found. */
struct acpi_sdt_header *acpi_get_table(const char sig[4]);


#endif /* ACPI_H */
