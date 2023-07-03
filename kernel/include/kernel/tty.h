#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H
#include <stdint.h>
#include <stddef.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
char terminal_getchar();
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void terminal_write_uint32(uint32_t);

#endif
