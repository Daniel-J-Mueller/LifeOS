[BITS 64]

global context_switch
extern current_task

; void context_switch(struct task *next)
; rdi holds pointer to next task per SysV ABI
context_switch:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rdx, [rel current_task]
    mov [rdx], rsp              ; save old stack pointer
    mov [rel current_task], rdi ; switch current task pointer
    mov rsp, [rdi]              ; load new stack pointer

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
