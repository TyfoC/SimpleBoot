#pragma once
#ifndef A20_LINE_H
#define A20_LINE_H

/*
	References:
		https://wiki.osdev.org/A20_Line
*/

#include "typedefs.h"

#define A20_LINE_SUPPORT_KBD_CTRL						1
#define A20_LINE_SUPPORT_FAST_GATE						2

extern uint8_t a20_line_is_enabled(void);
extern uint8_t a20_line_query_gate_support(void);
extern uint8_t a20_line_enable_using_bios(void);
extern void a20_line_enable_using_fast_gate(void);
extern void a20_line_enable_using_kbd_ctrl(void);

#endif