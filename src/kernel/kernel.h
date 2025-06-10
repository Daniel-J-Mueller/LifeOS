#ifndef KERNEL_H
#define KERNEL_H

/* High level kernel initialization entry points */

/* Primary C entry point invoked from assembly stub */
void kmain(void);

/* Sets up core subsystems before enabling tasks */
void kernel_init(void);

/* Halt the operating system */
void kernel_halt(void);
void kernel_reboot(void);
void kernel_shutdown(void);

#endif /* KERNEL_H */
