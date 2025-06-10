#include "keyboard.h"
#include "../../hal/io.h"
#include "../../kernel/driver/driver.h"

/* Partial scancode to ASCII table for set 1 */
static const char scancode_ascii[128] = {
    [0x1e] = 'a', [0x30] = 'b', [0x2e] = 'c', [0x20] = 'd',
    [0x12] = 'e', [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
    [0x17] = 'i', [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x32] = 'm', [0x31] = 'n', [0x18] = 'o', [0x19] = 'p',
    [0x10] = 'q', [0x13] = 'r', [0x1f] = 's', [0x14] = 't',
    [0x16] = 'u', [0x2f] = 'v', [0x11] = 'w', [0x2d] = 'x',
    [0x15] = 'y', [0x2c] = 'z',
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0a] = '9', [0x0b] = '0',
    [0x39] = ' ', [0x1c] = '\n', [0x0f] = '\t', [0x0e] = '\b'
};

char keyboard_read_char(void) {
    if ((inb(0x64) & 1) == 0)
        return 0;
    unsigned char sc = inb(0x60);
    if (sc >= sizeof(scancode_ascii))
        return 0;
    return scancode_ascii[sc];
}

static void keyboard_init(void) {
    /* No initialization needed for the stub driver */
}

static struct driver kbd_driver = {
    .name = "keyboard",
    .init = keyboard_init,
    .next = 0
};

void keyboard_register(void) {
    driver_register(&kbd_driver);
}
