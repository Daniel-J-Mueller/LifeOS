[BITS 16]
[ORG 0x7C00]

; LifeOS bootloader enabling 64-bit long mode and jumping to the kernel

start:
    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00
    mov [boot_drive], dl

    mov si, load_msg
    call print_string

    ; Load kernel from disk to 0x0000:0x1000
    mov ax, 0x0000
    mov es, ax
    mov bx, 0x1000
    mov ah, 0x02        ; BIOS read sectors
    mov al, 32          ; number of sectors
    mov ch, 0
    mov dh, 0
    mov cl, 2           ; start reading after boot sector
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    ; Set up 64-bit GDT
    lgdt [gdt_descriptor]

    ; Enter protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE32_SEL:protected_mode

[BITS 32]
protected_mode:
    mov ax, DATA32_SEL
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x7C00

    ; Build identity mapped paging structures for first 2 MiB
    mov eax, pdpt
    or eax, 0x03
    mov [pml4], eax
    mov dword [pml4+4], 0
    mov eax, pd
    or eax, 0x03
    mov [pdpt], eax
    mov dword [pdpt+4], 0
    mov dword [pd], 0x00000083
    mov dword [pd+4], 0

    ; Load paging structures
    mov eax, pml4
    mov cr3, eax

    ; Enable PAE
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; Jump to 64-bit mode
    jmp CODE64_SEL:long_mode_start

[BITS 64]
long_mode_start:
    mov ax, DATA64_SEL
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x90000

    ; Transfer control to the kernel
    jmp 0x1000

disk_error:
    mov si, err_msg
    call print_string
    jmp $

; Simple BIOS teletype output (16-bit real mode only)
print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp print_string
.done:
    ret

boot_drive db 0
err_msg   db 'Disk read failure', 0
load_msg  db 'Loading LifeOS kernel...', 0

align 8
gdt_start:
    dq 0x0000000000000000        ; null descriptor
    dq 0x00AF9A000000FFFF        ; 64-bit code segment
    dq 0x00AF92000000FFFF        ; 64-bit data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE32_SEL equ 0x08
DATA32_SEL equ 0x10
CODE64_SEL equ 0x08
DATA64_SEL equ 0x10

pml4   equ 0x8000
pdpt   equ 0x9000
pd     equ 0xA000

times 510-($-$$) db 0
dw 0xAA55

