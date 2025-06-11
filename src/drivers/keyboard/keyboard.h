#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Register the PS/2 keyboard driver */
void keyboard_register(void);

/* Read a character if available, 0 if none */
char keyboard_read_char(void);

/* Special key codes returned by keyboard_read_char */
#define KEY_UP    ((char)0x80)
#define KEY_DOWN  ((char)0x81)
#define KEY_LEFT  ((char)0x82)
#define KEY_RIGHT ((char)0x83)
#define KEY_F1    ((char)0x84)
#define KEY_F2    ((char)0x85)
#define KEY_F3    ((char)0x86)
#define KEY_F4    ((char)0x87)
#define KEY_CTRL  ((char)0x88)
#define KEY_ALT_R ((char)0x89)
#define KEY_FN    ((char)0x8A)

#endif /* KEYBOARD_H */
