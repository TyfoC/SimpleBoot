%define SECTOR_SIZE						512
%define MAX_SECTOR_NUMBER_MASK			63

;	dl - disk index
DiskDrvSelectDisk:
	push es
	mov ah, 0x08
	mov [DiskParameters.Index], dl
	mov di, 0
	mov es, di
	int 0x13
	pop es
	jc .Error

	test ah, ah
	jnz .Error

	and cl, MAX_SECTOR_NUMBER_MASK
	mov [DiskParameters.LastSectorNumber], cl
	mov [DiskParameters.LastHeadIndex], dl
	
	clc
	ret
.Error:
	stc
	ret

;	cx - sector[0-5] & cylinder[H:6-7][L:8-15]
;	dh - head index
DiskDrvIncCHS:
	push ax
	mov al, cl
	and al, MAX_SECTOR_NUMBER_MASK
	cmp al, [DiskParameters.LastSectorNumber]
	pop ax

	jae .IncHead
.IncSector:
	inc cl
	ret
.IncHead:
	cmp dh, [DiskParameters.LastHeadIndex]
	jae .IncCylinder

	inc dh
	and cl, 0xC0
	or cl, 1

	ret
.IncCylinder:
	xchg ch, cl
	shr ch, 6
	inc cx
	xchg cl, ch
	shl cl, 6
	mov dh, 0
	or cl, 1

	ret

;	es:bx - buffer
DiskDrvIncBuffer:
	push eax
	mov eax, 0

	push es
	pop ax
	shl eax, 4
	mov ax, bx
	add eax, 0x200
	mov bx, ax

	and eax, 0xF0000
	shr eax, 4
	mov es, ax

	pop eax
	ret

;	cx - sector[0-5] & cylinder[H:6-7][L:8-15]
;	dh - head index
;	es:bx - dest buffer
;	si - number of sectors
DiskDrvReadSectors:
	clc
.ReadSector:
	test si, si
	jz .Done

	mov ax, 0x0201
	mov dl, [DiskParameters.Index]
	int 0x13
	jc .Error

	test ah, ah
	jnz .Error

	call DiskDrvIncCHS
	call DiskDrvIncBuffer

	dec si
	jmp .ReadSector
.Done:
	ret
.Error:
	stc
	ret

DiskParameters:
.Index:				db 0
.LastSectorNumber:	db MAX_SECTOR_NUMBER_MASK		; Sectors/Track
.LastHeadIndex:		db 0x0F		; Heads/Cylinder - 1