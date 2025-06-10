[BITS 16]
[ORG 0x7c00]

; Bootloader Skeleton for LifeOS
; Sets up a minimal environment and transfers control to the kernel.

start:
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00
    mov [boot_drive], dl

    mov si, load_msg
    call print_string

    ; Load kernel from disk to 0x0000:0x1000
    mov ax, 0x0000
    mov es, ax
    mov bx, 0x1000
    mov ah, 0x02        ; BIOS read sectors
    mov al, 16          ; number of sectors
    mov ch, 0
    mov dh, 0
    mov cl, 2           ; start reading after boot sector
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    jmp 0x0000:0x1000

disk_error:
    mov si, err_msg
    call print_string

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

boot_drive db 0
err_msg   db 'Disk read failure', 0

load_msg db 'Loading LifeOS kernel...', 0

times 510-($-$$) db 0
dw 0xaa55

