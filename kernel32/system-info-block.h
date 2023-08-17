#pragma once
#ifndef SYSTEM_INFO_BLOCK_H
#define SYSTEM_INFO_BLOCK_H

#include "typedefs.h"

typedef struct {
	uint32_t	MemoryMapAddress;
	uint32_t	MemoryMapLength;				// number of regions
	uint32_t	PCIHardwareCharacteristics;
	uint32_t	BIOSDiskIndex;
	uint32_t	VideoBufferAddress;
} SystemInfoBlock_t;

#endif