#ifndef KERNEL_H
#define KERNEL_H

/* High level kernel initialization entry points */

/* Primary C entry point invoked from assembly stub */
void kmain(void);

/* Sets up core subsystems before enabling tasks */
void kernel_init(void);

#endif /* KERNEL_H */
