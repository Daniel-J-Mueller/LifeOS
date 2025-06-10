#ifndef HAL_CPU_H
#define HAL_CPU_H

#include <stdint.h>

/*
 * hal_cpu_core_count returns the number of logical CPU cores detected.
 * The implementation uses the CPUID instruction where available. If
 * detection fails, a single core is assumed.
 */
static inline unsigned int hal_cpu_core_count(void) {
    unsigned int eax, ebx, ecx, edx;
    unsigned int cores = 1;

    /* CPUID leaf 0x0B on modern Intel, fallback to leaf 1 otherwise */
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(0x0B), "c"(0)
                     :);
    if (ebx != 0) {
        cores = ebx & 0xFFFF;
    } else {
        __asm__ volatile("cpuid"
                         : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                         : "a"(1)
                         :);
        cores = (ebx >> 16) & 0xFF;
        if (cores == 0)
            cores = 1;
    }
    return cores;
}

#endif /* HAL_CPU_H */
