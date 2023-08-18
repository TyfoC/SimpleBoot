#pragma once
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define ASM(...)							__asm__ __volatile__(__VA_ARGS__)
#define ATTRIBUTE(...)						__attribute__((__VA_ARGS__))

#define STOP(...)							ASM("jmp ."::__VA_ARGS__)
#define CLI(...)							ASM("cli"::__VA_ARGS__)
#define HLT(...)							ASM("hlt"::__VA_ARGS__)
#define HALT(...)							ASM("cli; hlt"::__VA_ARGS__)

//	round value down ($alignment = 2^n)
#define FSTRNDDW($value, $alignment)		(($value) & ~(($alignment) - 1))

//	round value up ($alignment = 2^n)
#define FSTRNDUP($value, $alignment)		((($value) + ($alignment) - 1) & ~(($alignment) - 1))

//	round the value down and divide it by the $alignment ($alignment = 2^n)
#define FSTDIVDW($value, $alignment)		(FSTRNDDW($value, $alignment) / ($alignment))

//	round the value up and divide it by the $alignment ($alignment = 2^n)
#define FSTDIVUP($value, $alignment)		(FSTRNDUP($value, $alignment) / ($alignment))

#define PROGRAM_BASE_ADDRESS				((size_t)&__PTR_BASE_ADDRESS__)
#define PROGRAM_END_ADDRESS					((size_t)&__PTR_END_ADDRESS__)
#define PROGRAM_SIZE						(PROGRAM_END_ADDRESS - PROGRAM_BASE_ADDRESS)
#define PROGRAM_VER							0x01'05'00										// 1.4.0
#define PROGRAM_VER_STR						"1.5.0"

#define SECTOR_SIZE							512

#define KILOBYTE							0x400
#define KIBIBYTE							0x1000
#define MEGABYTE							0xF4240
#define MEBIBYTE							0x100000

#define RGB($red, $green, $blue)			(($red << 16) | ($green << 8) | ($blue))

extern void* __PTR_BASE_ADDRESS__;
extern void* __PTR_END_ADDRESS__;

#endif