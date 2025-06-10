[BITS 16]
[ORG 0x7c00]

; Bootloader Skeleton for LifeOS
; Sets up a minimal environment and transfers control to the kernel.

start:
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00

    mov si, load_msg
    call print_string

    ; TODO: load kernel image into memory
    ; This is a placeholder for disk loading logic.

hang:
    jmp hang

; Simple BIOS teletype output
print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    mov bh, 0x00
    int 0x10
    jmp print_string
.done:
    ret

load_msg db 'Loading LifeOS kernel...', 0

times 510-($-$$) db 0
word 0xaa55
