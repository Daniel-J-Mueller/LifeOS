#ifndef CONSOLE_H
#define CONSOLE_H

void console_init(void);
void console_clear(void);
void console_putc(char c);
void console_write(const char *s);

/* Poll keyboard driver and queue any typed characters */
void console_poll_input(void);

/* Retrieve a queued character, or 0 if none available */
char console_getc(void);

#endif /* CONSOLE_H */
