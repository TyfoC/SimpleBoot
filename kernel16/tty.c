#include "tty.h"

static char tmp_buffer_[66];
static uint8_t color_ = 0x07;
static size_t line_break_length_ = 1;

void tty_putc(char character) {
	if (character == '\t') for (uint8_t i = 0; i < TTY_TAB_LEN; i++) tty_putc(' ');
	else if (character == '\v') for (uint8_t i = 0; i < TTY_TAB_LEN; i++) tty_putc('\n');
	else if (character == '\n') {
		for (size_t i = 0; i < line_break_length_; i++) ASM("int $0x10"::"a"(0x0E0A), "b"(color_));
	}
	else if (character != '\b') ASM("int $0x10"::"a"(0x0E00 | (uint8_t)character), "b"(color_));
}

void tty_puts(const char* srcStr) {
	char* ptr = (char*)&srcStr[0];
	while (*ptr) tty_putc(*ptr++);
}

/*
	%c - character
	%s - string
	%d - signed decimal
	%u - unsigned decimal
	%x - unsigned hexadecimal
	%o - unsigned octal
	%b - unsigned binary
	%TW - word (text)
	%TD - double word (text)
	%TQ - quad (text)
*/
void tty_printf(const char* fmt, ...) {
	size_t* ptrArg = (size_t*)((size_t)&fmt + sizeof(size_t));
	char* ptrFmt = (char*)&fmt[0];
	while (*ptrFmt) {
		if (*ptrFmt == '%') {
			if (*(++ptrFmt) == '%') tty_putc('%');
			else if (*ptrFmt == 'c') tty_putc(*ptrArg++);
			else if (*ptrFmt == 's') tty_puts((char*)*ptrArg++);
			else if (*ptrFmt == 'd') {
				ltoa(*ptrArg++, tmp_buffer_, 10);
				tty_puts(tmp_buffer_);
			}
			else if (*ptrFmt == 'u') {
				ultoa(*ptrArg++, tmp_buffer_, 10);
				tty_puts(tmp_buffer_);
			}
			else if (*ptrFmt == 'x') {
				ultoa(*ptrArg++, tmp_buffer_, 16);
				tty_puts(tmp_buffer_);
			}
			else if (*ptrFmt == 'o') {
				ultoa(*ptrArg++, tmp_buffer_, 8);
				tty_puts(tmp_buffer_);
			}
			else if (*ptrFmt == 'b') {
				ultoa(*ptrArg++, tmp_buffer_, 2);
				tty_puts(tmp_buffer_);
			}
			else if (*ptrFmt == 'T') {
				size_t strLen = 0;
				if (*(++ptrFmt) == 'W') strLen = 2;
				else if (*ptrFmt == 'D') strLen = 4;
				else if (*ptrFmt == 'Q') strLen = 8;
				else --ptrFmt;

				tmp_buffer_[strLen] = '\0';
				memcpy(&tmp_buffer_[0], ptrArg++, strLen);
				tty_puts(tmp_buffer_);
			}
		}
		else tty_putc(*ptrFmt);

		++ptrFmt;
	}
}

void tty_set_color(uint8_t color) {
	color_ = color;
}

uint8_t tty_get_color(void) {
	return color_;
}

void tty_set_line_break_length(size_t lineBrkLen) {
	line_break_length_ = lineBrkLen;
}

uint8_t tty_get_line_break_length(void) {
	return line_break_length_;
}