[bits 16]

[global cpu_far_jmp]
cpu_far_jmp:							; void cpu_far_jmp(uint32_t farPtr)
	pop eax
	mov ax, [esp + 2]
	shl ax, 12
	push ax
	mov bx, [esp + 2]
	push bx
	retf

ALIGN(4)
PGDT:
	dw GDTEnd - GDTNull - 1
	dd GDTNull
GDTNull:		dq 0					; --	0x00
GDTPMCode:		dq 0xCF9A000000FFFF		; PM	0x08
GDTPMData:		dq 0xCF92000000FFFF		; PM	0x10
GDTPM16Code:	dq 0xF9A000000FFFF		; PM16	0x18
GDTPM16Data:	dq 0xF92000000FFFF		; PM16	0x20
GDTEnd:

[global __cpu_mode_rm_to_pm]
__cpu_mode_rm_to_pm:					; void __cpu_mode_rm_to_pm(uint32_t returnAddress)
	cli
	lgdt [PGDT]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp (GDTPMCode - GDTNull):.PM
[bits 32]
.PM:
	mov ax, GDTPMData - GDTNull
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	pop dword [.Tmp]
	pop dword [.Tmp]
	jmp dword [.Tmp]
.Tmp:	dd 0
[bits 16]

[bits 32]
[global __cpu_mode_pm_to_rm]
__cpu_mode_pm_to_rm:					; void __cpu_mode_pm_to_rm(uint32_t returnAddress)
	cli
	lgdt [PGDT]
	jmp (GDTPM16Code - GDTNull):.PM16
[bits 16]
.PM16:
	mov ax, GDTPM16Data - GDTNull
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov eax, cr0
	and eax, 0x7FFFFFFE
	mov cr0, eax
	jmp 0:.RM
.RM:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	sti

	pop dword [.Tmp]
	pop dword [.Tmp]
	shl word [.Tmp + 2], 12
	push word [.Tmp + 2]
	push word [.Tmp]
	retf
.Tmp:	dd 0