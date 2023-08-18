#pragma once
#ifndef TTY_H
#define TTY_H

#include <stdlib.h>
#include <string.h>
#include "mem.h"

#define TTY_TAB_LEN			4

void tty_putc(char character);
void tty_puts(const char* srcStr);
void tty_printf(const char* fmt, ...);

//	only for graphics video modes (VGA, VESA)
void tty_set_color(uint8_t color);
uint8_t tty_get_color(void);
void tty_set_line_break_length(size_t lineBrkLen);
uint8_t tty_get_line_break_length(void);

#endif