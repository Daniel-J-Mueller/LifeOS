#include "acpi.h"
#ifndef ACPI_TEST
#include "../console/console.h"
#else
static void console_write(const char *s) { (void)s; }
#endif

static struct acpi_fadt fadt_data;
static struct acpi_fadt *fadt_table = 0;

void acpi_init(void) {
    console_write("ACPI parser stub initialized\n");
    /* Real implementation will locate the RSDP and parse tables. */
    fadt_data.pm1a_cnt_blk = 0x604;
    fadt_data.pm1b_cnt_blk = 0xB004;
    fadt_data.slp_typa = 0x0;
    fadt_data.slp_typb = 0x0;
    fadt_table = &fadt_data;
}

struct acpi_fadt *acpi_get_fadt(void) {
    return fadt_table;
}
