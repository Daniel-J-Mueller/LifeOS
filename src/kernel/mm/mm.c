#include <stdint.h>
#include "mm.h"

#define PAGE_SIZE 4096

extern uint8_t end; /* Symbol defined by linker */

static uint64_t next_free_page;

/*
 * Initialize a very small physical page allocator. The allocator simply
 * hands out pages sequentially starting after the kernel image.
 */
void mm_init(void) {
    next_free_page = ((uint64_t)&end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    /*
     * The bootloader places its paging structures at fixed physical
     * addresses starting at 0x8000. Ensure the simple allocator does not
     * hand out pages that would overwrite these tables. Bump the first
     * free page to 0xB000 if the kernel image ends before that location.
     */
    if (next_free_page < 0xB000)
        next_free_page = 0xB000;
}

void *page_alloc(void) {
    void *page = (void *)next_free_page;
    next_free_page += PAGE_SIZE;
    return page;
}
