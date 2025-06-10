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

#endif /* KEYBOARD_H */
