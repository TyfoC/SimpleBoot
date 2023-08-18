#pragma once
#ifndef VESA_H
#define VESA_H

/*
	References:
		https://www.phatcode.net/res/221/files/vbe20.pdf
*/

#include <stddef.h>
#include <stdint.h>

#define VESA_CAPABILITY_DAC_WIDTH_SWITCHABLE			0x01			// 6 bits/color -> 8 bits/color
#define VESA_CAPABILITY_VGA_INCOMPATIBLE				0x02

//	 When programming large blocks of information to the RAMDAC, use the blank bit in Function 09h
#define VESA_CAPABILITY_SPECIFIC_RAMDAC					0x03

#define VESA_MODE_ATTRIBUTE_SUPPORTED_IN_HRDWR			0x01

// If set, the functions 1h, 2h, 6h, 7h, 9h, Ah, Eh int 10h are available
#define VESA_MODE_ATTRIBUTE_TTY_OUTPUT_AVAILABLE		0x04
#define VESA_MODE_ATTRIBUTE_COLOR						0x08			// monochrome if clear
#define VESA_MODE_ATTRIBUTE_GRAPHICS					0x10			// text if clear
#define VESA_MODE_ATTRIBUTE_VGA_INCOMPATIBLE			0x20
#define VESA_MODE_ATTRIBUTE_VGA_WINDOWED_NOT_AVAILABLE	0x40
#define VESA_MODE_ATTRIBUTE_LFB							0x80

#define VESA_MODE_MODEL_TEXT							0x00
#define VESA_MODE_MODEL_CGA_GRAPHICS					0x01
#define VESA_MODE_MODEL_HERCULES_GRAPHICS				0x02
#define VESA_MODE_MODEL_PLANAR							0x03
#define VESA_MODE_MODEL_PACKED_PIXEL					0x04
#define VESA_MODE_MODEL_256_COLOR						0x05
#define VESA_MODE_MODEL_DIRECT_COLOR					0x06
#define VESA_MODE_MODEL_YUV								0x07

typedef struct __attribute__((__packed__)) {
	uint32_t		Signature;
	uint16_t		Version;
	uint16_t		OEMStringsOffset;
	uint16_t		OEMStringsSegment;
	uint32_t		Capabilities;
	uint16_t		VideoModeOffset;
	uint16_t		VideoModeSegment;
	uint16_t		TotalMemorySize;		// in 64KB blocks
	uint8_t			Reserved[492];
} VESAInfo_t;

typedef struct __attribute__((__packed__)) {
	uint16_t		Attributes;				// VESA_MODE_ATTRIBUTE_*
	uint8_t			FirstWindow;
	uint8_t			SecondWindow;
	uint16_t		Granularity;
	uint16_t		WindowSize;
	uint16_t		FirstSegment;
	uint16_t		SecondSegment;
	uint32_t		WindowFunctionPointer;
	uint16_t		Pitch;					// bytes per scanline
	uint16_t		Width;
	uint16_t		Height;
	uint8_t			CharWidth;
	uint8_t			CharHeight;
	uint8_t			NumberOfPlanes;
	uint8_t			BitsPerPixel;
	uint8_t			NumberOfBanks;
	uint8_t			MemoryModel;			// VESA_MODE_MODEL_*
	uint8_t			BankSize;
	uint8_t			NumberOfImagePages;
	uint8_t			Reserved1;

	uint8_t			RedMaskSize;
	uint8_t			RedFieldPosition;
	uint8_t			GreenMaskSize;
	uint8_t			GreenFieldPosition;
	uint8_t			BlueMaskSize;
	uint8_t			BlueFieldPosition;
	uint8_t			ReservedMaskSize;
	uint8_t			ReservedFieldPosition;
	uint8_t			DirectColorModeInfo;

	// since VBE 2.0
	uint32_t		LFBPointer;
	uint32_t		OffscreenMemoryOffset;
	uint16_t		OffscreenMemorySize;	// in KB blocks
	uint8_t			Reserved2[206];
} VESAModeInfo_t;

uint8_t vesa_init(void);
VESAInfo_t* vesa_get_info(void);
VESAModeInfo_t* vesa_get_mode_info(uint16_t modeNumber);
uint16_t vesa_search_mode(uint16_t width, uint16_t height, uint8_t bitsPerPixel, uint8_t attributes, uint8_t memModel, uint32_t* lfbAddr);
extern uint8_t vesa_set_mode(uint16_t modeNumber);

#endif