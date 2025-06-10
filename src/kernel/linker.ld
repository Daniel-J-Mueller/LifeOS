ENTRY(start)
OUTPUT_ARCH(i386:x86-64)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x1000;
    .text : {
        *(.text*)
    }
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
    }
    .bss : {
        *(.bss*) *(COMMON)
    }
    end = .;
}
