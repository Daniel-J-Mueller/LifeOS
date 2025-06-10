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
}

void *page_alloc(void) {
    void *page = (void *)next_free_page;
    next_free_page += PAGE_SIZE;
    return page;
}
