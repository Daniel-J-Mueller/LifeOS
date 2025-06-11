#include <assert.h>
#include <string.h>
#include "../src/kernel/acpi/acpi.h"

unsigned char test_mem[512];
unsigned char *acpi_test_mem_start = test_mem;
unsigned int acpi_test_mem_size = sizeof(test_mem);

struct rsdp2 {
    char signature[8];
    unsigned char checksum;
    char oem_id[6];
    unsigned char revision;
    unsigned int rsdt_address;
    unsigned int length;
    unsigned long long xsdt_address;
    unsigned char ext_checksum;
    unsigned char reserved[3];
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
    struct rsdp2 *rsdp = (struct rsdp2 *)(test_mem + 16);
    memset(rsdp, 0, sizeof(*rsdp));
    memcpy(rsdp->signature, "RSD PTR ", 8);
    rsdp->revision = 2;
    rsdp->xsdt_address = 128;
    rsdp->length = sizeof(struct rsdp2);

    struct acpi_sdt_header *xsdt = (struct acpi_sdt_header *)(test_mem + 128);
    memcpy(xsdt->signature, "XSDT", 4);
    xsdt->length = sizeof(struct acpi_sdt_header) + 8;
    unsigned long long *entry = (unsigned long long *)((char *)xsdt + sizeof(struct acpi_sdt_header));
    *entry = 192;

    struct fadt_table *fadt = (struct fadt_table *)(test_mem + 192);
    memset(fadt, 0, sizeof(struct fadt_table));
    memcpy(fadt->h.signature, "FACP", 4);
    fadt->h.length = sizeof(struct fadt_table);
    fadt->pm1a_cnt_blk = 0xAAAA;
    fadt->pm1b_cnt_blk = 0xBBBB;

    acpi_init();
    struct acpi_fadt *f = acpi_get_fadt();
    assert(f);
    assert(f->pm1a_cnt_blk == 0xAAAA);
    assert(f->pm1b_cnt_blk == 0xBBBB);
    struct acpi_sdt_header *hdr = acpi_get_table("FACP");
    assert(hdr);
    return 0;
}
