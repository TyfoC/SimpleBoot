#pragma once
#ifndef BIOS_H
#define BIOS_H

#include "typedefs.h"

//	call bios interrupt (__VA_ARGS__ - input registers)
#define BIOSINT($intIndex, ...)	{ASM(""::__VA_ARGS__); __bios_int($intIndex);}

extern void __bios_int(uint8_t intIndex);

#endif