#include "strlib.h"

static const char digits_[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

size_t strlen(void* srcStr) {
	size_t strLen = 0;
	char* ptr = (char*)srcStr;
	while (ptr[strLen]) ++strLen;
	return strLen;
}

char* strrev(char* srcStr) {
	size_t strLen = strlen(srcStr);
	char tmp;
	for (size_t i = 0, j = strLen - 1; i < j; i++, j--) {
		tmp = srcStr[i];
		srcStr[i] = srcStr[j];
		srcStr[j] = tmp;
	}

	return srcStr;
}

void ltoa(ptrdiff_t value, uint8_t radix, void* dst) {
	if (radix < 2 || radix > 16) return;
	
	char* origBuff = (char*)dst;
	uint8_t negVal = value < 0;
	if (negVal) ++origBuff;

	size_t length = 1;
	ptrdiff_t tmpVal = value;
	while (tmpVal /= radix) ++length;

	origBuff = &origBuff[length];
	*origBuff-- = 0;
	while (length) {
		*origBuff-- = digits_[value % radix];
		value /= radix;
		--length;
	}

	if (negVal) *origBuff = '-';
}

void ultoa(size_t value, uint8_t radix, void* dst) {
	if (radix < 2 || radix > 16) return;

	char* origBuff = (char*)dst;

	size_t length = 1;
	size_t tmpVal = value;
	while (tmpVal /= radix) ++length;

	origBuff = &origBuff[length];
	*origBuff-- = 0;
	while (length) {
		*origBuff-- = digits_[value % radix];
		value /= radix;
		--length;
	}
}