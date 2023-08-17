#include "kernel-boot-block.h"
#include "system-info-block.h"

#include "tty.h"
#include "disk.h"
#include "mem.h"
#include "pci.h"
#include "a20-line.h"
#include "vesa.h"
#include "cpu.h"

#define KERNEL16_TMP_DST_ADDR								0x20000
#define THIRD_BOOTLDR_BASE_ADDR								0x3F000

/*
	Made by TyfoC
	Date: 16 Aug 2023
	Ver: 1.1.0
*/

VESAModeInfo_t VESAModeInfo __attribute__((__aligned__(16)));
KernelBootBlock_t KernelBootBlock __attribute__((__aligned__(16)));
SystemInfoBlock_t SystemInfoBlock __attribute__((__aligned__(16)));
uint32_t KernelHeaderValues[3] __attribute__((__aligned__(4)));

EXTERN_C void bootloader_main(uint8_t diskIndex, uint32_t kbbLBA, uint32_t kernelSectorLBA) {
	SystemInfoBlock.BIOSDiskIndex = (uint32_t)diskIndex;

	tty_puts("A20 line: ");
	uint8_t a20LineStatus = a20_line_is_enabled();
	if (a20LineStatus) tty_puts("enabled\r\n");
	else {
		tty_puts("disabled\r\n");

		if (a20_line_enable_using_bios()) tty_puts("A20 Line enabled using BIOS\r\n");
		else {
			tty_puts("Warning: failed to enable A20 line using BIOS!\r\n");

			uint8_t a20LineSupport = a20_line_query_gate_support() & 3;
			if (!a20LineSupport) {
				tty_puts("Warning: unable to detect methods to enable A20 line using BIOS!\r\n");
				a20LineSupport = 3;
			}
			else {
				tty_puts("A20 line gate support:\r\n");
				if (a20LineSupport & A20_LINE_SUPPORT_KBD_CTRL) tty_puts("+\tKeyboard controller\r\n");
				else if (a20LineSupport & A20_LINE_SUPPORT_FAST_GATE) tty_puts("+\tFast gate (bit #1 port 0x92)\r\n");
			}
				
			if (a20LineSupport & A20_LINE_SUPPORT_FAST_GATE) {
				tty_puts("Trying to enable A20 line using fast gate...\t");
				a20_line_enable_using_fast_gate();

				a20LineStatus = a20_line_is_enabled();
				if (a20LineStatus) tty_puts(a20LineStatus ? "success\r\n" : "failed\r\n");
			}

			if (!a20LineStatus) {
				if (a20LineSupport & A20_LINE_SUPPORT_KBD_CTRL) {
					a20_line_enable_using_kbd_ctrl();
					tty_puts("Trying to enable A20 line using keyboard controller...\t");

					a20LineStatus = a20_line_is_enabled();
					if (a20LineStatus) tty_puts(a20LineStatus ? "success\r\n" : "failed\r\n");
				}
			}
		}
	}

	if (!disk_select(diskIndex)) {
		tty_printf("Error: failed to identify disk (BIOS disk index: 0x%x)\r\n", diskIndex);
		HALT();
	}

	tty_printf("Disk index = 0x%x\r\n", diskIndex);
	tty_printf("\tSectors/Track: %u\r\n", disk_get_sectors_per_track());
	tty_printf("\tHeads/Cylinder: %u\r\n", disk_get_heads_per_cylinder());
	tty_printf("\tNumber of cylinders: %u\r\n", disk_get_number_of_cylinders());

	if (!disk_read_sector_lba(kbbLBA, 1, (uint32_t)&KernelBootBlock)) {
		tty_puts("Error: failed to read kernel boot block!\r\n");
		HALT();
	}

	if (disk_read_sector_ext(kernelSectorLBA, 1, PROGRAM_END_ADDRESS) != 1) {
		tty_puts("Error: failed to read kernel header!\r\n");
		HALT();
	}
	
	mem_copy_longs_from_pm(
		PROGRAM_END_ADDRESS, (uint32_t)&KernelHeaderValues[0],
		sizeof(KernelHeaderValues) / sizeof(KernelHeaderValues[0])
	);

	tty_printf("Kernel base address: 0x%x\r\n", KernelHeaderValues[0]);
	tty_printf("Kernel length in sectors: %u\r\n", KernelHeaderValues[1]);
	tty_printf("Kernel entry point address: 0x%x\r\n", KernelHeaderValues[2]);

	if (
		disk_read_sector_ext(
			kernelSectorLBA, KernelHeaderValues[1],
			KernelBootBlock.CPUMode == 16 ? KERNEL16_TMP_DST_ADDR : KernelHeaderValues[0]
		) != KernelHeaderValues[1]
	) {
		tty_puts("Error: failed to read kernel!\r\n");
		HALT();
	}

	if (KernelBootBlock.CPUMode == 16) {
		if (disk_read_sector_ext(kernelSectorLBA - 1, 1, THIRD_BOOTLDR_BASE_ADDR) != 1) {
			tty_puts("Error: failed to read third bootloader!\r\n");
			HALT();
		}
	}

	if (KernelBootBlock.CPUMode != 16 && KernelBootBlock.CPUMode != 32) {
		tty_puts("Error: unable to determine target CPU mode for kernel!\r\n");
		HALT();
	}
	
	if (
		KernelBootBlock.CPUMode == 16 &&
		(
			KernelHeaderValues[2] >= 0x10000 ||
			KernelHeaderValues[2] + KernelHeaderValues[1] * SECTOR_SIZE > 0x10000
		)
	) {
		tty_puts("Error: due to GCC limitations, the real mode kernel cannot be above the first segment!\r\n");
		HALT();
	}

	if (!pci_init()) tty_puts("Warning: failed to check PCI installation!\r\n");
	else {
		tty_printf("PCI protected mode entry point: 0x%x\r\n", pci_get_pm_entry_point());

		SystemInfoBlock.PCIHardwareCharacteristics = (uint32_t)pci_get_hardware_characteristics();
		if (SystemInfoBlock.PCIHardwareCharacteristics) {
			tty_puts("PCI hardware characteristics:\r\n");
			if (
				SystemInfoBlock.PCIHardwareCharacteristics & PCI_HRDWR_CHAR_ACC_MECH_1
			) tty_puts("+\tSpace access mechanism #1\r\n");
			else if (
				SystemInfoBlock.PCIHardwareCharacteristics & PCI_HRDWR_CHAR_ACC_MECH_2
			) tty_puts("+\tSpace access mechanism #2\r\n");
			else if (
				SystemInfoBlock.PCIHardwareCharacteristics & PCI_HRDWR_CHAR_SPEC_CYCLE_MECH_1
			) tty_puts("+\tSpecial cycle gen mechanism #1\r\n");
			else if (
				SystemInfoBlock.PCIHardwareCharacteristics & PCI_HRDWR_CHAR_SPEC_CYCLE_MECH_2
			) tty_puts("+\tSpecial cycle gen mechanism #2\r\n");
		}
	}

	SystemInfoBlock.MemoryMapAddress = PROGRAM_END_ADDRESS;
	SystemInfoBlock.MemoryMapLength = mem_get_map(SystemInfoBlock.MemoryMapAddress);
	tty_printf("Number of memory regions: %u\r\n", SystemInfoBlock.MemoryMapLength);

	tty_puts("VESA: ");
	VESAInfo_t* vesaInfo = 0;
	if (!vesa_init()) tty_puts("not available\r\n");
	else {
		tty_puts("available\r\n");

		vesaInfo = vesa_get_info();
		tty_printf("\tSignature: %TD\r\n", vesaInfo->Signature);
		tty_printf("\tVersion: %u.%u\r\n", vesaInfo->Version >> 8, vesaInfo->Version & 0xFF);

		if (vesaInfo->Capabilities) {
			tty_puts("\tCapabilities:\r\n");
			if (
				vesaInfo->Capabilities & VESA_CAPABILITY_DAC_WIDTH_SWITCHABLE
			) tty_puts("\t+\tDAC is fixed width, with 6 bits per primary color\r\n");
			else tty_puts("\t+\tDAC width is switchable to 8 bits per primary color\r\n");

			if (
				vesaInfo->Capabilities & VESA_CAPABILITY_VGA_INCOMPATIBLE
			) tty_puts("\t+\tController is not VGA compatible\r\n");
			else tty_puts("\t+\tController is VGA compatible\r\n");

			if (
				vesaInfo->Capabilities & VESA_CAPABILITY_SPECIFIC_RAMDAC
			) tty_puts("\t+\tNon-standard RAMDAC programming\r\n");
			else tty_puts("\t+\tNormal RAMDAC operation\r\n");
		}

		tty_printf("\tList of video modes: 0x%x\r\n", (uint32_t)((vesaInfo->VideoModeSegment << 4) + vesaInfo->VideoModeOffset));
		tty_printf("\tTotal mem size: 0x%x(*64KB)\r\n", vesaInfo->TotalMemorySize);
	}

	if (
		KernelBootBlock.VideoModeType == KBB_VIDEO_MODE_TYPE_TEXT &&
		KernelBootBlock.VideoModeWidth == 80 &&
		KernelBootBlock.VideoModeHeight == 25 &&
		KernelBootBlock.VideoModeBitsPerPixel == 8
	) {
		ASM("int $0x10"::"a"(0x0003));
		SystemInfoBlock.VideoBufferAddress = 0xB8000;
	}
	else if (
		KernelBootBlock.VideoModeType == KBB_VIDEO_MODE_TYPE_VGA &&
		KernelBootBlock.VideoModeWidth == 320 &&
		KernelBootBlock.VideoModeHeight == 200 &&
		KernelBootBlock.VideoModeBitsPerPixel == 8
	) {
		ASM("int $0x10"::"a"(0x0013));
		SystemInfoBlock.VideoBufferAddress = 0xA0000;
	}
	else if (KernelBootBlock.VideoModeType == KBB_VIDEO_MODE_TYPE_VESA)  {
		uint16_t videoModeNumber = vesa_search_mode(
			KernelBootBlock.VideoModeWidth,
			KernelBootBlock.VideoModeHeight,
			KernelBootBlock.VideoModeBitsPerPixel,
			KernelBootBlock.VideoModeAttributes,
			VESA_MODE_MODEL_PACKED_PIXEL,
			&SystemInfoBlock.VideoBufferAddress
		);

		if (videoModeNumber == 0xFFFF) {
			videoModeNumber = vesa_search_mode(
				KernelBootBlock.VideoModeWidth,
				KernelBootBlock.VideoModeHeight,
				KernelBootBlock.VideoModeBitsPerPixel,
				KernelBootBlock.VideoModeAttributes,
				VESA_MODE_MODEL_DIRECT_COLOR,
				&SystemInfoBlock.VideoBufferAddress
			);
		}

		if (videoModeNumber == 0xFFFF) {
			tty_puts("Error: failed to find selected VESA video mode!\r\n");
			HALT();
		}
		else {
			tty_printf("VESA video mode number: 0x%x\r\n", videoModeNumber);
			vesa_set_mode(videoModeNumber | 0x4000);
		}
	}
	else {
		tty_puts("Error: failed to select the given video mode for the kernel!\r\n");

		tty_printf("Video mode type: %u\r\n", KernelBootBlock.VideoModeType);
		tty_printf("Video mode width: %u\r\n", KernelBootBlock.VideoModeWidth);
		tty_printf("Video mode height: %u\r\n", KernelBootBlock.VideoModeHeight);
		tty_printf("Video mode bits/pixel: %u\r\n", KernelBootBlock.VideoModeBitsPerPixel);

		if (
			KernelBootBlock.VideoModeType == KBB_VIDEO_MODE_TYPE_VESA
		) tty_printf("Video mode attributes: %u\r\n", KernelBootBlock.VideoModeAttributes);

		HALT();
	}

	if (KernelBootBlock.CPUMode == 16) {
		ASM(
			"pushl %%esi;"
			"pushl %%eax;"
			"pushl %%ecx;"
			"pushl %%edx;"
			"pushl %%ebx;"
			"calll jmp_to_third_bootldr;"
			::"a"(KernelHeaderValues[1] * SECTOR_SIZE / 2),
			"c"(KernelHeaderValues[2]),
			"d"(KernelHeaderValues[0]),
			"b"(KERNEL16_TMP_DST_ADDR),
			"S"((uint32_t)&SystemInfoBlock)
		);
	}
	else {
		ASM(
			"andl $0xFFFF, %%ebx;"
			"pushl %%ebx;"
			"pushl $0;"
			"pushl %%eax;"
			"calll __cpu_mode_rm_to_pm;"
			::"a"(KernelHeaderValues[2]), "b"((size_t)&SystemInfoBlock)
		);
	}
}