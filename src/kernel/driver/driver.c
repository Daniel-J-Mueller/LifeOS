#include "driver.h"

static struct driver *drv_head = 0;
static struct driver *drv_tail = 0;

void driver_register(struct driver *drv) {
    drv->next = 0;
    if (!drv_head) {
        drv_head = drv_tail = drv;
    } else {
        drv_tail->next = drv;
        drv_tail = drv;
    }
}

void driver_init_all(void) {
    for (struct driver *d = drv_head; d; d = d->next) {
        if (d->init)
            d->init();
    }
}
