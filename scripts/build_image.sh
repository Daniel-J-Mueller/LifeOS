#!/usr/bin/env bash
set -e

# Build output directory
mkdir -p build

# Assemble kernel entry stub
nasm -f elf64 src/kernel/entry.asm -o build/entry.o

# Compile kernel sources
GCC=x86_64-linux-gnu-gcc
LD=x86_64-linux-gnu-ld
CFLAGS="-m64 -ffreestanding -Wall -Wextra -fno-pie -fno-stack-protector"

$GCC $CFLAGS -c src/kernel/main.c -o build/main.o
$GCC $CFLAGS -c src/kernel/init.c -o build/init.o
$GCC $CFLAGS -c src/kernel/mm/mm.c -o build/mm.o
$GCC $CFLAGS -c src/kernel/fs/fs.c -o build/fs.o
$GCC $CFLAGS -c src/kernel/console/console.c -o build/console.o
$GCC $CFLAGS -c src/kernel/console/cmd.c -o build/cmd.o
$GCC $CFLAGS -c src/kernel/power.c -o build/power.o
$GCC $CFLAGS -c src/kernel/acpi/acpi.c -o build/acpi.o
$GCC $CFLAGS -c src/lib/string.c -o build/string.o
$GCC $CFLAGS -c src/kernel/console/shell.c -o build/shell.o
$GCC $CFLAGS -c src/kernel/inventory/inventory.c -o build/inventory.o
$GCC $CFLAGS -c src/kernel/inventory/pci_classes.c -o build/pci_classes.o
$GCC $CFLAGS -c src/kernel/sched/sched.c -o build/sched.o
$GCC $CFLAGS -c src/kernel/driver/driver.c -o build/driver.o
$GCC $CFLAGS -c src/kernel/syscall/syscall.c -o build/syscall.o
$GCC $CFLAGS -c src/drivers/keyboard/keyboard.c -o build/keyboard.o
$GCC $CFLAGS -c src/kernel/hmi/pane.c -o build/pane.o
$GCC $CFLAGS -c src/gui/framebuffer.c -o build/framebuffer.o
$GCC $CFLAGS -c src/gui/gui.c -o build/gui.o
nasm -f elf64 src/kernel/sched/context_switch.asm -o build/context_switch.o

# Link kernel binary

$LD -m elf_x86_64 -nostdlib -T src/kernel/linker.ld \
    build/entry.o build/init.o build/main.o \
    build/mm.o build/fs.o build/console.o build/shell.o build/cmd.o build/power.o build/acpi.o build/string.o build/inventory.o build/pci_classes.o build/sched.o build/driver.o build/syscall.o build/keyboard.o build/pane.o build/context_switch.o \
    build/framebuffer.o build/gui.o \
    -o build/kernel.bin

# Determine number of sectors the bootloader must load
KERNEL_SIZE=$(stat -c %s build/kernel.bin)
KERNEL_SECTORS=$(( (KERNEL_SIZE + 511) / 512 ))

# Assemble bootloader with calculated sector count
nasm -f bin -DKERNEL_SECTORS=$KERNEL_SECTORS src/boot/bootloader.asm -o build/bootloader.bin

# Combine bootloader and kernel into a bootable image
cat build/bootloader.bin build/kernel.bin > build/os-image.bin

echo "Build complete. Bootable image located at build/os-image.bin"
