#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

struct task {
    uint64_t *sp;           /* Saved stack pointer */
    struct task *next;      /* Next task in run queue */
};

void sched_init(void);
struct task *sched_create_task(void (*entry)(void));
void sched_add_task(struct task *t);
void sched_yield(void);
void sched_start(void);

#endif /* SCHED_H */
