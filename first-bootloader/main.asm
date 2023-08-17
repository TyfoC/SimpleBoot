bits 16
org 0x7FC00

%define HEADER_BASE_SEGMENT									0x0000
%define HEADER_BASE_OFFSET									0x1000

%define SECOND_BOOTLDR_SECTOR_CYLINDER						0x0004
%define SECOND_BOOTLDR_HEAD									0

%define KBB_SECTOR_LBA										0x00000002

%define KBB_VIDEO_MODE_TYPE_TEXT							0x01
%define KBB_VIDEO_MODE_TYPE_VGA								0x02
%define KBB_VIDEO_MODE_TYPE_VESA							0x03

; If set, the functions 1h, 2h, 6h, 7h, 9h, Ah, Eh int 10h are available
%define KBB_VIDEO_MODE_ATTRIBUTE_TTY_OUTPUT_AVAILABLE		0x04
%define KBB_VIDEO_MODE_ATTRIBUTE_COLOR						0x08			; monochrome if clear
%define KBB_VIDEO_MODE_ATTRIBUTE_GRAPHICS					0x10			; text if clear
%define KBB_VIDEO_MODE_ATTRIBUTE_VGA_INCOMPATIBLE			0x20
%define KBB_VIDEO_MODE_ATTRIBUTE_VGA_WINDOWED_NOT_AVAILABLE	0x40
%define KBB_VIDEO_MODE_ATTRIBUTE_LFB						0x80

Begin:
	pop dword [.TmpKernelLBA]

	call DiskDrvSelectDisk
	jc .DiskInitError

	; set default video mode (80x25)
	mov al, 0x03
	int 0x10

	; disable cursor blinking
	mov ax, 0x1003
	mov bx, 0
	int 0x10
	
	mov si, MsgLoading
	call TtyPutString

	mov bx, HEADER_BASE_OFFSET
	push word HEADER_BASE_SEGMENT
	pop es

	push bx
	push es
	mov cx, SECOND_BOOTLDR_SECTOR_CYLINDER
	mov dh, SECOND_BOOTLDR_HEAD
	mov si, 1
	call DiskDrvReadSectors
	pop es
	pop bx
	jc .DiskReadError

	mov si, [es:bx + 2]
	mov bx, [es:bx]
	push bx
	push es
	mov cx, SECOND_BOOTLDR_SECTOR_CYLINDER
	mov dh, SECOND_BOOTLDR_HEAD
	call DiskDrvReadSectors
	pop es
	pop bx
	jc .DiskReadError

	mov dx, es
	mov ss, dx
	mov bp, bx
	mov sp, bp

	push dword [.TmpKernelLBA]					; kernel sector LBA
	push dword KBB_SECTOR_LBA					; kernel boot block sector LBA

	mov edx, 0
	mov dl, [DiskParameters.Index]
	push edx									; disk index

	mov dx, es
	mov ds, dx
	mov fs, dx
	mov gs, dx

	push dword 0								; return address
	push word HEADER_BASE_SEGMENT
	push word [es:bx + 4]
	retf										; void bootloader_main(void* arguments, size_t count);

.DiskInitError:
	mov esi, MsgDiskInitError
	jmp .Error
.DiskReadError:
	mov esi, MsgDiskReadError
.Error:
	call TtyPutString
	cli
	hlt
	jmp $
.TmpKernelLBA:	dd 0

MsgLoading:			db "Loading main part...", 13, 10, 0
MsgDiskInitError:	db "Error: failed to init disk!", 0
MsgDiskReadError:	db "Error: failed to read disk!", 0

%include "tty.asm"
%include "disk.asm"

times 512 - $ + $$ db 0
End:

KernelBootBlock:
%include "kernel-boot-cfg.asm"
times 1024 - $ + $$ db 0
KernelBootBlockEnd: