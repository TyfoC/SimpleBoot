[bits 16]
[extern __cpu_mode_rm_to_pm]
[extern __cpu_mode_pm_to_rm]

[global __mem_get_map_entry_e820]
__mem_get_map_entry_e820:						; uint8_t __mem_get_map_entry_e820(uint32_t dstFarPtr, uint32_t* entryOffset)
	push es
	mov eax, 0xE820
	mov ecx, 0x24
	mov edx, 0x534D4150
	mov ebx, [esp + 10]
	mov ebx, [ebx]
	mov di, [esp + 8]
	shl di, 12
	mov es, di
	mov di, [esp + 6]
	int 0x15
	jc .Error

	cmp eax, 0x534D4150
	jne .Error

	mov eax, [esp + 10]
	mov [eax], ebx
	mov al, 1
	jmp .Done
.Error:
	mov al, 0
.Done:
	pop es
	retd

[global  __mem_e801]
 __mem_e801:									; uint16_t  __mem_e801(void)
 	mov eax, 0xE801
	int 0x15
	jc .Error

	jcxz .UseAX
	jmp .Done
.Error:
	mov eax, 0
	retd
.UseAX:
	mov ax, cx
	mov bx, dx
.Done:
	xchg ax, bx
	shl eax, 16
	mov ax, bx
	retd

[global mem_copy_longs_from_pm]
mem_copy_longs_from_pm:						; void mem_copy_longs_from_pm(uint32_t srcAddr, uint32_t dstAddr, uint32_t longsCount)
	push eax
	push ecx
	push esi
	push edi

	mov esi, [esp + 20]
	mov edi, [esp + 24]
	mov ecx, [esp + 28]

	push dword .PM
	call dword __cpu_mode_rm_to_pm
[bits 32]
.PM:
	test ecx, ecx
	jz .Done

	mov eax, [esi]
	mov [edi], eax

	add esi, 4
	add edi, 4
	dec ecx
	jmp .PM
.Done:
	push dword .RM
	call dword __cpu_mode_pm_to_rm
[bits 16]
.RM:
	pop edi
	pop esi
	pop ecx
	pop eax
	retd

[global mem_copy_words_from_pm]
mem_copy_words_from_pm:						; void mem_copy_words_from_pm(uint32_t srcAddr, uint32_t dstAddr, uint32_t wordsCount)
	push eax
	push ecx
	push esi
	push edi

	mov esi, [esp + 20]
	mov edi, [esp + 24]
	mov ecx, [esp + 28]

	push dword .PM
	call dword __cpu_mode_rm_to_pm
[bits 32]
.PM:
	test ecx, ecx
	jz .Done

	mov ax, [esi]
	mov [edi], ax

	add esi, 2
	add edi, 2
	dec ecx
	jmp .PM
.Done:
	push dword .RM
	call dword __cpu_mode_pm_to_rm
[bits 16]
.RM:
	pop edi
	pop esi
	pop ecx
	pop eax
	retd

[global mem_copy_bytes_from_pm]
mem_copy_bytes_from_pm:						; void mem_copy_bytes_from_pm(uint32_t srcAddr, uint32_t dstAddr, uint32_t bytesCount)
	push eax
	push ecx
	push esi
	push edi

	mov esi, [esp + 20]
	mov edi, [esp + 24]
	mov ecx, [esp + 28]

	push dword .PM
	call dword __cpu_mode_rm_to_pm
[bits 32]
.PM:
	test ecx, ecx
	jz .Done

	mov al, [esi]
	mov [edi], al

	inc esi
	inc edi
	dec ecx
	jmp .PM
.Done:
	push dword .RM
	call dword __cpu_mode_pm_to_rm
[bits 16]
.RM:
	pop edi
	pop esi
	pop ecx
	pop eax
	retd

[global __mem_init_entry]
__mem_init_entry:								; void __mem_init_entry(uint32_t dstFarPtr, uint32_t addr, uint32_t size)
	push ds
	mov eax, [esp + 6]
	mov si, ax
	shr eax, 4
	and eax, 0xF000
	mov ds, ax

	mov eax, [esp + 10]
	mov [ds:si], eax
	mov dword [ds:si + 4], 0
	mov eax, [esp + 14]
	mov [ds:si + 8], eax
	mov dword [ds:si + 12], 0
	mov dword [ds:si + 16], 1
	mov dword [ds:si + 20], 1
	pop ds
	retd