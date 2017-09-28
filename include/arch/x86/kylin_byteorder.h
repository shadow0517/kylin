#ifndef _KYLIN_ARCH_BYTEORDER_X86_64_H_
#define _KYLIN_ARCH_BYTEORDER_X86_64_H_

#include <kylin/include/arch/common/kylin_byteorder.h>

#ifndef KYLIN_BYTE_ORDER
#define KYLIN_BYTE_ORDER KYLIN_LITTLE_ENDIAN
#endif

/*
 * An architecture-optimized byte swap for a 16-bit value.
 *
 * Do not use this function directly. The preferred function is kylin_bswap16().
 */
static inline uint16_t kylin_arch_bswap16(uint16_t _x)
{
	register uint16_t x = _x;
	asm volatile ("xchgb %b[x1],%h[x2]"
		      : [x1] "=Q" (x)
		      : [x2] "0" (x)
		      );
	return x;
}

/*
 * An architecture-optimized byte swap for a 32-bit value.
 *
 * Do not use this function directly. The preferred function is kylin_bswap32().
 */
static inline uint32_t kylin_arch_bswap32(uint32_t _x)
{
	register uint32_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}

#define kylin_bswap16(x) ((uint16_t)(__builtin_constant_p(x) ?		\
				   kylin_constant_bswap16(x) :		\
				   kylin_arch_bswap16(x)))

#define kylin_bswap32(x) ((uint32_t)(__builtin_constant_p(x) ?		\
				   kylin_constant_bswap32(x) :		\
				   kylin_arch_bswap32(x)))

#define kylin_bswap64(x) ((uint64_t)(__builtin_constant_p(x) ?		\
				   kylin_constant_bswap64(x) :		\
				   kylin_arch_bswap64(x)))

#define kylin_cpu_to_le_16(x) (x)
#define kylin_cpu_to_le_32(x) (x)
#define kylin_cpu_to_le_64(x) (x)

#define kylin_cpu_to_be_16(x) kylin_bswap16(x)
#define kylin_cpu_to_be_32(x) kylin_bswap32(x)
#define kylin_cpu_to_be_64(x) kylin_bswap64(x)

#define kylin_le_to_cpu_16(x) (x)
#define kylin_le_to_cpu_32(x) (x)
#define kylin_le_to_cpu_64(x) (x)

#define kylin_be_to_cpu_16(x) kylin_bswap16(x)
#define kylin_be_to_cpu_32(x) kylin_bswap32(x)
#define kylin_be_to_cpu_64(x) kylin_bswap64(x)

/*
 * An architecture-optimized byte swap for a 64-bit value.
 *
  * Do not use this function directly. The preferred function is kylin_bswap64().
 */
/* 64-bit mode */
static inline uint64_t kylin_arch_bswap64(uint64_t _x)
{
	register uint64_t x = _x;
	asm volatile ("bswap %[x]"
		      : [x] "+r" (x)
		      );
	return x;
}

#endif /*_KYLIN_ARCH_BYTEORDER_X86_64_H_*/
