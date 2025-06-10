
[BITS 16]
[ORG 0x1000]

extern kmain

; Kernel entry stub for LifeOS
; Switches from real mode to 32-bit protected mode and calls kmain.

start:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEL:protected_mode_start

[BITS 32]
protected_mode_start:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    call kmain

.halt:
    hlt
    jmp .halt

; Global Descriptor Table
align 8
gdt_start:
    dq 0x0000000000000000        ; null descriptor
    dq 0x00CF9A000000FFFF        ; code segment
    dq 0x00CF92000000FFFF        ; data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEL equ 0x08
DATA_SEL equ 0x10
