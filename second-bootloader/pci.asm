[bits 16]

%define PCI_CHARS_SIGN		0x20494350

[global __pci_get_info]
__pci_get_info:				; uint8_t __pci_get_info(uint8_t* info, uint32_t* entryPoint)
	pushf
	pushad
	mov ax, 0xB101
	mov edi, 0
	int 0x1A
	jc .Error

	test ah, ah
	jnz .Error

	cmp edx, PCI_CHARS_SIGN
	jne .Error

	mov [.TmpReg1], edi
	mov [.TmpReg2], al
	popad
	popf

	push eax
	push ebx
	push edi
	mov ebx, [esp + 16]
	mov edi, [esp + 20]
	mov eax, [.TmpReg1]
	mov [edi], eax
	mov eax, [.TmpReg2]
	mov [ebx], eax
	pop edi
	pop ebx
	pop eax

	mov al, 1
	retd
.Error:
	popad
	popf
	mov al, 0
	retd
.TmpReg1:		dd 0
.TmpReg2:		dd 0