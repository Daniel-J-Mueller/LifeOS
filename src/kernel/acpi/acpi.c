#include "acpi.h"
#include <stddef.h>
#ifndef ACPI_TEST
#include "../console/console.h"
#else
static void console_write(const char *s) { (void)s; }
/* Test harness provides the BIOS memory region to scan */
extern unsigned char *acpi_test_mem_start;
extern unsigned int acpi_test_mem_size;
#endif

static struct acpi_fadt fadt_data;
static struct acpi_fadt *fadt_table = 0;

struct acpi_rsdp {
    char     signature[8];
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_address;
} __attribute__((packed));

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

struct acpi_fadt_table {
    struct acpi_sdt_header h;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t  reserved1;
    uint8_t  preferred_pm_profile;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t  acpi_enable;
    uint8_t  acpi_disable;
    uint8_t  S4bios_req;
    uint8_t  pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
} __attribute__((packed));

void acpi_init(void) {
    console_write("ACPI parser initializing\n");

    unsigned char *start;
    unsigned char *end;
#ifdef ACPI_TEST
    start = acpi_test_mem_start;
    end = acpi_test_mem_start + acpi_test_mem_size;
#else
    start = (unsigned char *)0xE0000;
    end = (unsigned char *)0x100000;
#endif

    const char sig[8] = "RSD PTR ";
    for (unsigned char *p = start; p + sizeof(struct acpi_rsdp) <= end; p += 16) {
        int match = 1;
        for (unsigned int i = 0; i < 8; i++) {
            if (p[i] != sig[i]) { match = 0; break; }
        }
        if (!match)
            continue;

        struct acpi_rsdp *rsdp = (struct acpi_rsdp *)p;
        struct acpi_sdt_header *rsdt =
#ifdef ACPI_TEST
            (struct acpi_sdt_header *)(start + rsdp->rsdt_address);
#else
            (struct acpi_sdt_header *)(uintptr_t)rsdp->rsdt_address;
#endif

        if (rsdt->signature[0] != 'R' || rsdt->signature[1] != 'S' ||
            rsdt->signature[2] != 'D' || rsdt->signature[3] != 'T')
            break;

        unsigned int entries =
            (rsdt->length - sizeof(struct acpi_sdt_header)) / 4;
        uint32_t *addrs = (uint32_t *)(rsdt + 1);
        for (unsigned int i = 0; i < entries; i++) {
            struct acpi_sdt_header *hdr =
#ifdef ACPI_TEST
                (struct acpi_sdt_header *)(start + addrs[i]);
#else
                (struct acpi_sdt_header *)(uintptr_t)addrs[i];
#endif
            if (hdr->signature[0]=='F' && hdr->signature[1]=='A' &&
                hdr->signature[2]=='C' && hdr->signature[3]=='P') {
                struct acpi_fadt_table *fadt = (struct acpi_fadt_table *)hdr;
                fadt_data.pm1a_cnt_blk = (uint16_t)fadt->pm1a_cnt_blk;
                fadt_data.pm1b_cnt_blk = (uint16_t)fadt->pm1b_cnt_blk;
                fadt_data.slp_typa = 0;
                fadt_data.slp_typb = 0;
                fadt_table = &fadt_data;
                console_write("ACPI tables parsed\n");
                return;
            }
        }
    }

    /* Fallback defaults when tables are not found */
    fadt_data.pm1a_cnt_blk = 0x604;
    fadt_data.pm1b_cnt_blk = 0xB004;
    fadt_data.slp_typa = 0x0;
    fadt_data.slp_typb = 0x0;
    fadt_table = &fadt_data;
    console_write("ACPI tables not found, using defaults\n");
}

struct acpi_fadt *acpi_get_fadt(void) {
    return fadt_table;
}
