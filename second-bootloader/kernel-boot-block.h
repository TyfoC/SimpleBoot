#pragma once
#ifndef KERNEL_BOOT_BLOCK_H
#define KERNEL_BOOT_BLOCK_H

#include "typedefs.h"

#define KBB_VIDEO_MODE_TYPE_TEXT					1
#define KBB_VIDEO_MODE_TYPE_VGA						2
#define KBB_VIDEO_MODE_TYPE_VESA					3

typedef struct __attribute__((__packed__)) {
	uint16_t			CPUMode;					// 16 - Real mode, 32 - protected mode
	uint16_t			VideoModeType;
	uint16_t			VideoModeWidth;
	uint16_t			VideoModeHeight;
	uint16_t			VideoModeBitsPerPixel;
	uint16_t			VideoModeAttributes;		// only available if VideoModeType == VESA
	uint8_t				Reserved[502];
} KernelBootBlock_t;

#endif