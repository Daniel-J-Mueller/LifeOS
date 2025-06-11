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

static int have_e0 = 0;
static char handle_extended(unsigned char sc) {
    switch (sc) {
    case 0x48: return KEY_UP;
    case 0x50: return KEY_DOWN;
    case 0x4B: return KEY_LEFT;
    case 0x4D: return KEY_RIGHT;
    case 0x38: return KEY_ALT_R;
    case 0x5D: return KEY_FN;
    default:
        return 0;
    }
}

char keyboard_read_char(void) {
    if ((inb(0x64) & 1) == 0)
        return 0;
    unsigned char sc = inb(0x60);
    if (sc == 0xE0) {
        have_e0 = 1;
        return 0;
    }
    if (have_e0) {
        have_e0 = 0;
        return handle_extended(sc);
    }
    if (sc & 0x80)
        return 0;
    switch (sc) {
    case 0x3B: return KEY_F1;
    case 0x3C: return KEY_F2;
    case 0x3D: return KEY_F3;
    case 0x3E: return KEY_F4;
    case 0x3F: return KEY_F5;
    case 0x40: return KEY_F6;
    case 0x41: return KEY_F7;
    case 0x42: return KEY_F8;
    case 0x1D: return KEY_CTRL;
    case 0x38: return KEY_ALT_R; /* treat left alt same as right for now */
    default:
        break;
    }
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
