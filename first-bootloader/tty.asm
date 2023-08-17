;	al - character
TtyPutChar:
	push ax
	mov ah, 0x0E
	int 0x10
	pop ax
	ret

;	ds:si - str far ptr
TtyPutString:
	push ax
	push si
	push ds
	mov ah, 0x0E
.PutChar:
	lodsb
	test al, al
	jz .Done
	int 0x10
	jmp .PutChar
.Done:
	pop ds
	pop si
	pop ax
	ret