#pragma once
#ifndef STRLIB_H
#define STRLIB_H

#include "typedefs.h"

size_t strlen(void* srcStr);
char* strrev(char* srcStr);
void ltoa(ptrdiff_t value, uint8_t radix, void* dst);
void ultoa(size_t value, uint8_t radix, void* dst);

#endif