ENTRY(kernel16_main)
OUTPUT_ARCH(i386)
OUTPUT_FORMAT(binary)
SECTIONS {
	__PTR_BASE_ADDRESS__ = 0x6000;
	. = __PTR_BASE_ADDRESS__;

	.header : {
		LONG(__PTR_BASE_ADDRESS__);
		LONG(((__PTR_END_ADDRESS__ - __PTR_BASE_ADDRESS__ + 511) & ~511) / 512);
		LONG(kernel16_main);
	}

	.text : {
		*(.text)
	}

	.data BLOCK(512) : ALIGN(512) {
		*(.data)
	}

	.rodata BLOCK(512) : ALIGN(512) {
		*(.rodata)
	}

	.bss BLOCK(512) : ALIGN(512) {
		*(.bss)
	}

	.padding BLOCK(510) : ALIGN(510) {
		SHORT(0);
	}

	__PTR_END_ADDRESS__ = .;
}