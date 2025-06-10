#include "acpi.h"
#include "../console/console.h"

static void *fadt_table = 0;

void acpi_init(void) {
    console_write("ACPI parser stub initialized\n");
    /* Real implementation will locate the RSDP and parse tables. */
    fadt_table = 0;
}

void *acpi_get_fadt(void) {
    return fadt_table;
}
