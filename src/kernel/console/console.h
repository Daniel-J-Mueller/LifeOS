#ifndef CONSOLE_H
#define CONSOLE_H

void console_init(void);
void console_clear(void);
void console_putc(char c);
void console_write(const char *s);
void console_write_dec(unsigned long value);

/* Poll keyboard driver and queue any typed characters */
void console_poll_input(void);

/* Retrieve a queued character, or 0 if none available */
char console_getc(void);

/* Execute a command string */
void console_execute_command(const char *cmd);

#endif /* CONSOLE_H */
