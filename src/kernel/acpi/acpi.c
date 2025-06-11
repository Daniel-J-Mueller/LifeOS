#include "acpi.h"
#include <stddef.h>
#ifndef ACPI_TEST
#include "../console/console.h"
#else
static void console_write(const char *s) { (void)s; }
static void console_write_hex32(uint32_t v) { (void)v; }
static void console_write_hex16(uint16_t v) { (void)v; }
static void console_putc(char c) { (void)c; }
/* Test harness provides the BIOS memory region to scan */
extern unsigned char *acpi_test_mem_start;
extern unsigned int acpi_test_mem_size;
#endif

static struct acpi_fadt fadt_data;
static struct acpi_fadt *fadt_table = 0;

#define MAX_ACPI_TABLES 16
static struct acpi_sdt_header *acpi_tables[MAX_ACPI_TABLES];
static unsigned int acpi_table_count = 0;

/* ACPI 1.0 RSDP layout with optional 2.0 extensions */
struct acpi_rsdp {
    char     signature[8];
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_address;
    uint32_t length;       /* Only valid when revision >= 2 */
    uint64_t xsdt_address;  /* ACPI 2.0 64-bit table pointer */
    uint8_t  ext_checksum;  /* Checksum of entire structure */
    uint8_t  reserved[3];
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

static struct acpi_rsdp *scan_rsdp(unsigned char *s, unsigned char *e) {
    const char sig[8] = "RSD PTR ";
    for (unsigned char *p = s; p + sizeof(struct acpi_rsdp) <= e; p += 16) {
        int match = 1;
        for (unsigned int i = 0; i < 8; i++) {
            if (p[i] != sig[i]) { match = 0; break; }
        }
        if (match)
            return (struct acpi_rsdp *)p;
    }
    return NULL;
}

void acpi_init(void) {
    console_write("ACPI parser initializing\n");

    unsigned char *start;
    unsigned char *end;
    struct acpi_rsdp *rsdp = NULL;
#ifdef ACPI_TEST
    start = acpi_test_mem_start;
    end = acpi_test_mem_start + acpi_test_mem_size;
    rsdp = scan_rsdp(start, end);
#else
    uint16_t ebda_seg = *(volatile uint16_t *)(uintptr_t)0x40e;
    start = (unsigned char *)((uintptr_t)ebda_seg << 4);
    end = start + 1024;
    rsdp = scan_rsdp(start, end);
    if (!rsdp) {
        start = (unsigned char *)0xE0000;
        end = (unsigned char *)0x100000;
        rsdp = scan_rsdp(start, end);
    }
#endif

    if (rsdp) {
        console_write("RSDP found at 0x");
        console_write_hex32((uint32_t)(uintptr_t)rsdp);
        console_putc('\n');
        int use_xsdt = (rsdp->revision >= 2 && rsdp->xsdt_address);
        struct acpi_sdt_header *sdt = NULL;
#ifdef ACPI_TEST
        if (use_xsdt)
            sdt = (struct acpi_sdt_header *)(start + (uintptr_t)rsdp->xsdt_address);
        else
            sdt = (struct acpi_sdt_header *)(start + rsdp->rsdt_address);
#else
        if (use_xsdt && rsdp->xsdt_address < 0x200000)
            sdt = (struct acpi_sdt_header *)(uintptr_t)rsdp->xsdt_address;
        else if (!use_xsdt && rsdp->rsdt_address < 0x200000)
            sdt = (struct acpi_sdt_header *)(uintptr_t)rsdp->rsdt_address;
#endif
        if (sdt) {
            console_write(use_xsdt ? "XSDT at 0x" : "RSDT at 0x");
            console_write_hex32((uint32_t)(uintptr_t)sdt);
            console_putc('\n');
        }
        if (sdt && ((sdt->signature[0]=='R' && sdt->signature[1]=='S' && sdt->signature[2]=='D' && sdt->signature[3]=='T') ||
                    (sdt->signature[0]=='X' && sdt->signature[1]=='S' && sdt->signature[2]=='D' && sdt->signature[3]=='T'))) {
            acpi_table_count = 0;
            unsigned int entries = (sdt->length - sizeof(struct acpi_sdt_header)) / (use_xsdt ? 8 : 4);
            for (unsigned int i = 0; i < entries && acpi_table_count < MAX_ACPI_TABLES; i++) {
                struct acpi_sdt_header *hdr;
                uintptr_t addr;
                if (use_xsdt) {
                    uint64_t *addrs = (uint64_t *)(sdt + 1);
                    addr = (uintptr_t)addrs[i];
                } else {
                    uint32_t *addrs = (uint32_t *)(sdt + 1);
                    addr = (uintptr_t)addrs[i];
                }
#ifdef ACPI_TEST
                hdr = (struct acpi_sdt_header *)(start + addr);
#else
                hdr = (addr < 0x200000) ? (struct acpi_sdt_header *)addr : NULL;
#endif
                if (!hdr)
                    continue;
                acpi_tables[acpi_table_count++] = hdr;
                if (hdr->signature[0]=='F' && hdr->signature[1]=='A' &&
                    hdr->signature[2]=='C' && hdr->signature[3]=='P') {
                    struct acpi_fadt_table *fadt = (struct acpi_fadt_table *)hdr;
                    fadt_data.pm1a_cnt_blk = (uint16_t)fadt->pm1a_cnt_blk;
                    fadt_data.pm1b_cnt_blk = (uint16_t)fadt->pm1b_cnt_blk;
                    fadt_data.slp_typa = 0;
                    fadt_data.slp_typb = 0;
                    fadt_table = &fadt_data;
                    console_write("FADT at 0x");
                    console_write_hex32((uint32_t)addr);
                    console_write(" PM1a=");
                    console_write_hex16(fadt_data.pm1a_cnt_blk);
                    console_write(" PM1b=");
                    console_write_hex16(fadt_data.pm1b_cnt_blk);
                    console_putc('\n');
                }
            }
            console_write("ACPI tables parsed\n");
            return;
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

struct acpi_sdt_header *acpi_get_table(const char sig[4]) {
    for (unsigned int i = 0; i < acpi_table_count; i++) {
        struct acpi_sdt_header *h = acpi_tables[i];
        if (h->signature[0] == sig[0] && h->signature[1] == sig[1] &&
            h->signature[2] == sig[2] && h->signature[3] == sig[3])
            return h;
    }
    return NULL;
}
