[bits 16]
[extern __cpu_mode_rm_to_pm]
[extern __cpu_mode_pm_to_rm]

[global a20_line_is_enabled]
a20_line_is_enabled:							; uint8_t a20_line_is_enabled(void)
	push dword .Check
	call dword __cpu_mode_rm_to_pm
[bits 32]
.Check:
	push eax
	push esi
	push edi
	mov esi, 0x010000
	mov edi, 0x110000

	mov eax, [esi]
	mov [.ValueBelowMB], eax
	mov eax, [edi]
	mov [.ValueAboveMB], eax

	mov dword [esi], 0xAABBCCDD
	mov dword [edi], 0x11223344
	cmp dword [esi], 0x11223344
	je .Disabled

	mov dword [.Result], 1
	jmp .Checked
.Disabled:
	mov dword [.Result], 0
.Checked:

	mov eax, [.ValueAboveMB]
	mov [edi], eax
	mov eax, [.ValueBelowMB]
	mov [esi], eax

	pop edi
	pop esi
	pop eax

	push dword .Done
	call dword __cpu_mode_pm_to_rm
[bits 16]
.Done:
	mov al, [.Result]
	retd
.ValueBelowMB:	dd 0
.ValueAboveMB:	dd 0
.Result:		dd 0

[global a20_line_query_gate_support]
a20_line_query_gate_support:					; uint8_t a20_line_query_gate_support(void)
	mov ax, 0x2403
	int 0x15
	jc .Error

	test ah, ah
	jnz .Error

	mov al, bl
	and al, 3
	jmp .Done
.Error:
	mov al, 0
.Done:
	retd

[global a20_line_enable_using_bios]
a20_line_enable_using_bios:						; uint8_t a20_line_enable_using_bios(void)
	mov ax, 0x2401
	int 0x15
	jc .Error

	test ah, ah
	jnz .Error

	mov al, 1
	jmp .Done
.Error:
	mov al, 0
.Done:
	retd

[global a20_line_enable_using_fast_gate]
a20_line_enable_using_fast_gate:						; void a20_line_enable_using_fast_gate(void)
	in al, 0x92
	or al, 0x02
	and al, 0xFE
	out 0x92, al
	retd

[global a20_line_enable_using_kbd_ctrl]
a20_line_enable_using_kbd_ctrl:						; void a20_line_enable_using_kbd_ctrl(void)
.ReqStatus:
	call .WaitKbdCtrl
	mov al, 0xD1
	out 0x64, al
	call .WaitKbdCtrl
	mov al, 0xDF
	out 0x64, al
	call .WaitKbdCtrl
	retd
.WaitKbdCtrl:
	in al, 0x64
	test al, 2
	jnz .WaitKbdCtrl
	ret