[bits 16]

[global __bios_int]
__bios_int:				;	void __bios_int(uint8_t intIndex)
	push ax
	mov al, [esp + 6]
	mov [.call_int + 1], al
	pop ax
.call_int:	int 0xFF
	retd