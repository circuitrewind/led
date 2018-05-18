#ifndef __OPTIMIZE_H__
#define __OPTIMIZE_H__



#include "../color/inline.h"



#ifndef likely
#	if defined(__GNUC__)  ||  defined(__INTEL_COMPILER)
#		define likely(x)	__builtin_expect(!!(x),1)
#		define unlikely(x)	__builtin_expect(!!(x),0)
#	else
#		define likely(x)	(x)
#		define unlikely(x)	(x)
#	endif
#endif




//SOURCE:
//https://helloacm.com/how-to-reverse-bits-for-32-bit-unsigned-integer-in-cc/
INLINE uint32_t reverseBits(uint32_t n) {
	n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
	n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
	n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
	n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
	n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
	return n;
}



#endif //__OPTIMIZE_H__
