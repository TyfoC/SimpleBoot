#include "mem.h"

extern uint8_t __mem_get_map_entry_e820(uint32_t dstFarPtr, uint32_t* entryOffset);

// EAX[0-15] - number of kilobytes between 1MB and 16MB
// EAX[16-31] - number of kilobytes above 16M
extern uint16_t __mem_e801(void);
extern void __mem_init_entry(uint32_t dstFarPtr, uint32_t addr, uint32_t size);

void* memcpy(void* dst, const void* src, size_t size) {
	uint8_t* src8 = (uint8_t*)src;
	uint8_t* dst8 = (uint8_t*)dst;
	for (size_t i = 0; i < size; i++) dst8[i] = src8[i];
	return dst;
}

void* memset(void* dst, uint8_t value, size_t size) {
	uint8_t* dst8 = (uint8_t*)dst;
	for (size_t i = 0; i < size; i++) dst8[i] = value;
	return dst;
}

void memcpy_ext(uint32_t dstAddr, uint32_t srcAddr, uint32_t size) {
	uint32_t longParts = size / sizeof(uint32_t);
	size %= sizeof(uint32_t);
	size_t wordParts = size / sizeof(uint16_t);
	size_t byteParts = size %= sizeof(uint16_t);

	uint32_t longPartsSize = longParts * sizeof(uint32_t), wordPartsSize = wordParts * sizeof(uint16_t);

	mem_copy_longs_from_pm(srcAddr, dstAddr, longParts);
	mem_copy_words_from_pm(srcAddr += longPartsSize, dstAddr += longPartsSize, wordParts);
	mem_copy_bytes_from_pm(srcAddr += wordPartsSize, dstAddr += wordPartsSize, byteParts);
}

size_t mem_get_map(uint32_t dstAddr) {
	size_t mapLen = mem_get_map_e820(dstAddr);
	if (mapLen) return mapLen;

	__mem_init_entry(dstAddr, 0, 0x80000);
	mapLen = 1;
	dstAddr += sizeof(AddressRangeDescriptor_t);

	uint32_t e801Info = __mem_e801();
	if (e801Info) {
		__mem_init_entry(dstAddr, MEBIBYTE, (e801Info & 0xFFFF) * KILOBYTE);
		++mapLen;
		dstAddr += sizeof(AddressRangeDescriptor_t);

		e801Info >>= 16;
		if (e801Info) {
			__mem_init_entry(dstAddr, 0x1000000, (e801Info & 0xFFFF) * KILOBYTE);
			++mapLen;
			dstAddr += sizeof(AddressRangeDescriptor_t);
		}
	}

	return mapLen;
}

size_t mem_get_map_e820(uint32_t dstAddr) {
	uint8_t status = true;
	uint32_t entryOffset = 0;
	size_t mapLength = 0;
	do {
		status = __mem_get_map_entry_e820(dstAddr, &entryOffset);
		if (status) ++mapLength;
		dstAddr += sizeof(AddressRangeDescriptor_t);
	} while (status && entryOffset);

	return mapLength;
}