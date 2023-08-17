[bits 16]
org 0x7C00

%define PART_STATUS_BOOTABLE				0x80
%define PART_TYPE_ALT_OS					0x7F

%define MAIN_PHASE_SEGMENT					0x7000
%define MAIN_PHASE_OFFSET					0xFC00
%define MAIN_PHASE_SECTOR_CYLINDER			0x0002
%define MAIN_PHASE_LBA						0x00000001
%define MAIN_PHASE_HEAD						0

%define KERNEL_SECTOR_LBA					((Kernel - Begin) / 512)

%define HEADER_BASE_ADDR					0x1000

Begin:
	jmp 0:Main
Main:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov bp, HEADER_BASE_ADDR
	mov sp, bp
	sti
	cld

	mov ax, 0x0201
	mov cx, MAIN_PHASE_SECTOR_CYLINDER
	mov dh, MAIN_PHASE_HEAD
	mov bx, MAIN_PHASE_OFFSET
	push word MAIN_PHASE_SEGMENT
	pop es
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error

	mov bx, es
	mov ds, bx
	mov fs, bx
	mov gs, bx

	push dword KERNEL_SECTOR_LBA
	jmp MAIN_PHASE_SEGMENT:MAIN_PHASE_OFFSET
.Error:
	int 0x19
	jmp $
	
times 446 - $ + $$ db 0
times 510 - $ + $$ db 0
dw 0xAA55
End:

MainPhase:
incbin "first-bootloader-main.bin"
MainPhaseEnd:

SecondBootloader:
incbin "second-bootloader.bin"
SecondBootloaderEnd:

ThirdBootloader:
incbin "third-bootloader.bin"
ThirdBootloaderEnd:

Kernel:
incbin "kernel32.bin"
KernelEnd: