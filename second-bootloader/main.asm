[bits 16]

%define THIRD_BOOTLDR_SEGMENT				0x3F00
%define THIRD_BOOTLDR_OFFSET				0x0000

[global jmp_to_third_bootldr]
jmp_to_third_bootldr:	; void jmp_to_third_bootldr(uint32_t tmpKernelFarPtr, uint32_t dstKernelFarPtr, uint16_t dstReturnOff, uint16_t wordsCnt)
	pop esi

	mov si, [esp + 2]
	shl si, 12
	mov ds, si
	pop esi

	mov di, [esp + 2]
	shl di, 12
	mov es, di
	pop edi

	pop ebx
	pop ecx
	jmp THIRD_BOOTLDR_SEGMENT:THIRD_BOOTLDR_OFFSET
