[BITS 32]

global context_switch
extern current_task

; void context_switch(struct task *next)
context_switch:
    pushad                      ; save caller registers
    mov edx, [current_task]
    mov [edx], esp              ; save old stack pointer
    mov eax, [esp + 32]         ; load 'next' parameter (before pushad)
    mov [current_task], eax
    mov esp, [eax]              ; load new stack pointer
    popad                       ; restore registers of new task
    ret
