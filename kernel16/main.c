#include "system-info-block.h"
#include "tty.h"

/*
	Build & run & clear command:
	make build-kernel-cfg cpum=16 vmt=2 vmw=320 vmh=200 vmbpp=8 vma=0 && make build-os16 && make run-os16 && make clear-os16
*/
EXTERN_C void kernel16_main(SystemInfoBlock_t* systemInfoBlock) {
	tty_set_color(0x0E);
	tty_printf("Memory map: 0x%x\r\n", systemInfoBlock->MemoryMapAddress);
	tty_printf("Number of regions: %u\r\n", systemInfoBlock->MemoryMapLength);
	tty_printf("PCI hardware characteristics: 0b%b\r\n", systemInfoBlock->PCIHardwareCharacteristics);
	tty_printf("BIOS disk index: 0x%x\r\n", systemInfoBlock->BIOSDiskIndex);
	tty_printf("Video buffer address: 0x%x\r\n", systemInfoBlock->VideoBufferAddress);

	HALT();
}