#include "system-info-block.h"

/*
	Build & run & clear command:
	make build-kernel-cfg cpum=32 vmt=3 vmw=800 vmh=600 vmbpp=24 vma=144 && make build-os32 && make run-os32 && make clear-os32
*/
EXTERN_C void kernel32_main(SystemInfoBlock_t* systemInfoBlock) {
	uint8_t* videoBuffer = (uint8_t*)systemInfoBlock->VideoBufferAddress;

	uint32_t lineOffset = 0, columnOffset;
	uint8_t colorRed = 0x7F, colorGreen = 0x23, colorBlue = 0xBB;
	while (TRUE) {
		for (size_t y = 0; y < 600; y++) {
			for (columnOffset = 0; columnOffset < 800 * 3;) {
				videoBuffer[lineOffset + columnOffset++] = colorBlue;
				videoBuffer[lineOffset + columnOffset++] = colorGreen;
				videoBuffer[lineOffset + columnOffset++] = colorRed;
			}

			colorBlue++;
			colorGreen--;
			colorRed -= 3;

			lineOffset += 800 * 3;
			columnOffset = 0;
		}

		lineOffset = 0;
	}

	HALT();
}