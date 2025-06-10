#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Register the PS/2 keyboard driver */
void keyboard_register(void);

/* Read a character if available, 0 if none */
char keyboard_read_char(void);

#endif /* KEYBOARD_H */
