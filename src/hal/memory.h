#ifndef HAL_MEMORY_H
#define HAL_MEMORY_H

#include <stdint.h>

/*
 * hal_memory_size returns the total physical memory size in bytes.
 * This is currently a placeholder until real detection code exists.
 */
static inline uint64_t hal_memory_size(void) {
    return 64ULL * 1024 * 1024; /* 64 MiB stub */
}

#endif /* HAL_MEMORY_H */
