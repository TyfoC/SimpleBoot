#pragma once
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define FALSE			0
#define TRUE			1

#define _CONCAT($a, $b)	$a##$b
#define CONCAT($a, $b)	_CONCAT($a, $b)
#define _STRINGIFY(...)	#__VA_ARGS__
#define STRINGIFY(...)	_STRINGIFY(__VA_ARGS__)

#define EXTERN_C		extern

#define ASM(...)		__asm__ __volatile__(__VA_ARGS__)
#define ATTRIBUTE(...)	__attribute__((__VA_ARGS__))

#define STOP(...)		ASM("jmp ."::__VA_ARGS__)
#define CLI(...)		ASM("cli"::__VA_ARGS__)
#define HLT(...)		ASM("hlt"::__VA_ARGS__)
#define HALT(...)		ASM("cli; hlt"::__VA_ARGS__)

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
#define PROGRAM_VER							0x01'04'00										// 1.4.0
#define PROGRAM_VER_STR						"1.4.0"

#define SECTOR_SIZE							512

#define KILOBYTE							0x400
#define KIBIBYTE							0x1000
#define MEGABYTE							0xF4240
#define MEBIBYTE							0x100000

typedef char int8_t, *pint8_t;
typedef unsigned char uint8_t, *puint8_t;
typedef short int16_t, *pint16_t;
typedef unsigned short uint16_t, *puint16_t;
typedef int int32_t, *pint32_t;
typedef unsigned int uint32_t, *puint32_t;
typedef long long int64_t, *pint64_t;
typedef unsigned long long uint64_t, *puint64_t;

typedef __PTRDIFF_TYPE__ ptrdiff_t, *pptrdiff_t;
typedef __SIZE_TYPE__ size_t, *psize_t;

EXTERN_C void* __PTR_BASE_ADDRESS__;
EXTERN_C void* __PTR_END_ADDRESS__;

#endif