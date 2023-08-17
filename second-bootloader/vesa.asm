[bits 16]

[global __vesa_init]
__vesa_init:						; uint8_t __vesa_init(VESAInfo_t* info)
	mov eax, [esp + 4]
	push es
	push di
	mov di, ax
	
	shr eax, 4
	and eax, 0xF000
	
	mov ax, 0x4F00

	push ebp
	int 0x10
	pop ebp

	pop di
	pop es

	jc .Error

	mov al, 1
	jmp .Done
.Error:
	mov al, 0
.Done:
	retd

[global __vesa_get_mode_info]
__vesa_get_mode_info:				; uint8_t __vesa_get_mode_info(VESAModeInfo_t* modeInfo, uint16_t modeNumber)
	push cx
	push es
	push di
	mov cx, [esp + 14]

	mov eax, [esp + 10]
	mov di, ax
	
	shr eax, 4
	and eax, 0xF000
	
	mov ax, 0x4F01

	push ebp
	int 0x10
	pop ebp

	jc .Error

	mov al, 1
	jmp .Done
.Error:
	mov al, 0
.Done:
	pop di
	pop es
	pop cx
	retd

[global vesa_set_mode]
vesa_set_mode:						; uint8_t vesa_set_mode(uint16_t modeNumber)
	push bx
	mov ax, 0x4F02
	mov bx, [esp + 6]
	push ebp
	int 0x10
	pop ebp

	jc .Error

	mov al, 1
	jmp .Done
.Error:
	mov al, 0
.Done:
	pop bx
	retd