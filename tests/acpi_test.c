#include <assert.h>
#include "../src/kernel/acpi/acpi.h"


int main(void) {
    acpi_init();
    struct acpi_fadt *fadt = acpi_get_fadt();
    assert(fadt);
    assert(fadt->pm1a_cnt_blk == 0x604);
    assert(fadt->pm1b_cnt_blk == 0xB004);
    assert(fadt->slp_typa == 0x0);
    assert(fadt->slp_typb == 0x0);
    return 0;
}
