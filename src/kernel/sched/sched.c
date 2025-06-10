#include "../mm/mm.h"
#include "../console/console.h"
#include "sched.h"

extern void context_switch(struct task *next);

static struct task *task_list_head = 0;
static struct task *task_list_tail = 0;
struct task *current_task = 0;

/* Initialize scheduler structures */
void sched_init(void) {
    task_list_head = 0;
    task_list_tail = 0;
    current_task = 0;
}

/* Create a new task with its own stack */
struct task *sched_create_task(void (*entry)(void)) {
    struct task *t = (struct task *)page_alloc();
    uint8_t *stack = (uint8_t *)page_alloc();
    uint64_t *sp = (uint64_t *)(stack + 4096);

    /* Build initial stack so context_switch pops registers and jumps to entry */
    *--sp = (uint64_t)entry; /* return address for first switch */
    for (int i = 0; i < 6; ++i)
        *--sp = 0;           /* r15..rbp */

    t->sp = sp;
    t->next = 0;
    return t;
}

/* Add task to round-robin queue */
void sched_add_task(struct task *t) {
    if (!task_list_head) {
        task_list_head = task_list_tail = t;
    } else {
        task_list_tail->next = t;
        task_list_tail = t;
    }
}

/* Yield control to the next task */
void sched_yield(void) {
    if (!current_task || !current_task->next)
        current_task = task_list_head;
    else
        current_task = current_task->next;
    context_switch(current_task);
}

/* Begin round-robin task execution */
void sched_start(void) {
    if (!task_list_head)
        return;
    current_task = task_list_head;
    context_switch(current_task);
}
