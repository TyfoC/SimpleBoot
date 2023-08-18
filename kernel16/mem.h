#pragma once
#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "typedefs.h"

#define ADDRESS_RANGE_DESCRIPTOR_TYPE_USABLE			0x01
#define ADDRESS_RANGE_DESCRIPTOR_ATTRIBUTE_PRESENT		0x01

typedef struct __attribute__((__packed__)) {
	uint64_t	Address;
	uint64_t	Size;
	uint32_t	Type;
	uint32_t	Attributes;
} AddressRangeDescriptor_t;

/*
	srcAddr - src buffer address
	dstAddr - dst buffer address
*/
extern void mem_copy_longs_from_pm(uint32_t srcAddr, uint32_t dstAddr, uint32_t longsCount);
extern void mem_copy_words_from_pm(uint32_t srcAddr, uint32_t dstAddr, uint32_t wordsCount);
extern void mem_copy_bytes_from_pm(uint32_t srcAddr, uint32_t dstAddr, uint32_t bytesCount);

void memcpy_ext(uint32_t dstAddr, uint32_t srcAddr, uint32_t size);

/*
	dstAddr - far pointer (<=0xFFFFF)!
	Returns the number of regions
*/
size_t mem_get_map(uint32_t dstAddr);

/*
	dstAddr - far pointer (<=0xFFFFF)!
	Returns the number of regions
*/
size_t mem_get_map_e820(uint32_t dstAddr);

#endif