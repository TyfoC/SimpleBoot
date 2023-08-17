[bits 16]
[org 0x3F000]

;	ds:si - src far ptr
;	es:di - dst far ptr
;	es:bx - return far ptr
;	cx - size in words[2]
Begin:
	mov ax, es
	rep movsw

	mov ds, ax
	mov fs, ax
	mov gs, ax

	push dword 0
	push ax
	push bx
	retf

times 512 - $ + $$ db 0
End: