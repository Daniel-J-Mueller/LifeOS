#ifndef MM_H
#define MM_H

/* Initialize basic memory management structures */
void mm_init(void);

/* Allocate one 4 KiB page of memory */
void *page_alloc(void);

#endif /* MM_H */
