#include "vesa.h"

static VESAInfo_t info_ __attribute__((__aligned__(16)));
static VESAModeInfo_t mode_info_ __attribute__((__aligned__(16)));

extern uint8_t __vesa_init(VESAInfo_t* info);
extern uint8_t __vesa_get_mode_info(VESAModeInfo_t* modeInfo, uint16_t modeNumber);

uint8_t vesa_init(void) {
	return __vesa_init(&info_);
}

VESAInfo_t* vesa_get_info(void) {
	return &info_;
}

/*
	Returns 0 if failed
*/
VESAModeInfo_t* vesa_get_mode_info(uint16_t modeNumber) {
	return __vesa_get_mode_info(&mode_info_, modeNumber) ? &mode_info_ : 0;
}

/*
	attributes - VESA_MODE_ATTRIBUTE_*
	memModel - VESA_MODE_MODEL_*
	Returns 0xFFFF if the mode was not found
*/
uint16_t vesa_search_mode(uint16_t width, uint16_t height, uint8_t bitsPerPixel, uint8_t attributes, uint8_t memModel, uint32_t* lfbAddr) {
	if (info_.VideoModeOffset == info_.VideoModeSegment && info_.VideoModeOffset == 0) return 0xFFFF;

	uint16_t* videoModes = (uint16_t*)(size_t)info_.VideoModeOffset;
	while (*videoModes != 0xFFFF) {
		if (
			__vesa_get_mode_info(&mode_info_, *videoModes) &&
			mode_info_.Width == width &&
			mode_info_.Height == height &&
			mode_info_.BitsPerPixel == bitsPerPixel && 
			(mode_info_.Attributes & attributes) == attributes &&
			mode_info_.MemoryModel == memModel
		) {
			if (lfbAddr) *lfbAddr = mode_info_.LFBPointer;
			return *videoModes;
		}
		++videoModes;
	}

	return 0xFFFF;
}