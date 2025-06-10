
[BITS 64]
global start

extern kmain

; 64-bit kernel entry stub for LifeOS

start:
    cli
    lgdt [gdt_descriptor]

    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x90000

    call kmain

.halt:
    hlt
    jmp .halt

; Global Descriptor Table
align 8
gdt_start:
    dq 0x0000000000000000        ; null descriptor
    dq 0x00AF9A000000FFFF        ; 64-bit code segment
    dq 0x00AF92000000FFFF        ; 64-bit data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start

CODE_SEL equ 0x08
DATA_SEL equ 0x10
