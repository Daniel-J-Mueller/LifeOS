#ifndef DRIVER_H
#define DRIVER_H

struct driver {
    const char *name;
    void (*init)(void);
    struct driver *next;
};

void driver_register(struct driver *drv);
void driver_init_all(void);

#endif /* DRIVER_H */
