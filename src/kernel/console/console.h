#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void console_init(void);
void console_clear(void);
void console_putc(char c);
void console_write(const char *s);
void console_write_dec(unsigned long value);
void console_write_hex8(uint8_t value);
void console_write_hex16(uint16_t value);
void console_write_hex32(uint32_t value);
void console_write_hex64(uint64_t value);

/* Poll keyboard driver and queue any typed characters */
void console_poll_input(void);

/* Retrieve a queued character, or 0 if none available */
char console_getc(void);

/* Execute a command string */
void console_execute_command(const char *cmd);

/* Retrieve or set the cursor position */
void console_get_cursor(uint8_t *x, uint8_t *y);
void console_set_cursor(uint8_t x, uint8_t y);
void console_draw_quadrants(void);

#endif /* CONSOLE_H */
