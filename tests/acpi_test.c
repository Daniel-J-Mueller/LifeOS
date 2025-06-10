#include <assert.h>
#include <string.h>
#include "../src/kernel/acpi/acpi.h"

unsigned char test_mem[256];
unsigned char *acpi_test_mem_start = test_mem;
unsigned int acpi_test_mem_size = sizeof(test_mem);

struct rsdp {
    char     signature[8];
    unsigned char checksum;
    char     oem_id[6];
    unsigned char revision;
    unsigned int rsdt_address;
} __attribute__((packed));

struct acpi_sdt_header {
    char     signature[4];
    unsigned int length;
    unsigned char revision;
    unsigned char checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    unsigned int oem_revision;
    unsigned int creator_id;
    unsigned int creator_revision;
} __attribute__((packed));

struct fadt_table {
    struct acpi_sdt_header h;
    unsigned int firmware_ctrl;
    unsigned int dsdt;
    unsigned char reserved1;
    unsigned char preferred_pm_profile;
    unsigned short sci_int;
    unsigned int smi_cmd;
    unsigned char acpi_enable;
    unsigned char acpi_disable;
    unsigned char s4bios_req;
    unsigned char pstate_cnt;
    unsigned int pm1a_evt_blk;
    unsigned int pm1b_evt_blk;
    unsigned int pm1a_cnt_blk;
    unsigned int pm1b_cnt_blk;
};

int main(void) {
    struct rsdp *rsdp = (struct rsdp *)(test_mem + 16);
    memcpy(rsdp->signature, "RSD PTR ", 8);
    rsdp->rsdt_address = 64;

    struct acpi_sdt_header *rsdt = (struct acpi_sdt_header *)(test_mem + 64);
    memcpy(rsdt->signature, "RSDT", 4);
    rsdt->length = sizeof(struct acpi_sdt_header) + 4;
    unsigned int *entry = (unsigned int *)((char *)rsdt + sizeof(struct acpi_sdt_header));
    *entry = 128;

    struct fadt_table *fadt = (struct fadt_table *)(test_mem + 128);
    memset(fadt, 0, sizeof(struct fadt_table));
    memcpy(fadt->h.signature, "FACP", 4);
    fadt->h.length = sizeof(struct fadt_table);
    fadt->pm1a_cnt_blk = 0x1111;
    fadt->pm1b_cnt_blk = 0x2222;

    acpi_init();
    struct acpi_fadt *f = acpi_get_fadt();
    assert(f);
    assert(f->pm1a_cnt_blk == 0x1111);
    assert(f->pm1b_cnt_blk == 0x2222);
    assert(f->slp_typa == 0);
    assert(f->slp_typb == 0);
    return 0;
}
