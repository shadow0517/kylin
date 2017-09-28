#ifndef _KYLIN_ARCH_BYTEORDER_H_
#define _KYLIN_ARCH_BYTEORDER_H_

#include <kylin/include/kylin.h>

/*
 * Compile-time endianness detection
 */
#define KYLIN_BIG_ENDIAN    1
#define KYLIN_LITTLE_ENDIAN 2
#if defined __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define KYLIN_BYTE_ORDER KYLIN_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define KYLIN_BYTE_ORDER KYLIN_LITTLE_ENDIAN
#endif /* __BYTE_ORDER__ */
#elif defined __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
#define KYLIN_BYTE_ORDER KYLIN_BIG_ENDIAN
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define KYLIN_BYTE_ORDER KYLIN_LITTLE_ENDIAN
#endif /* __BYTE_ORDER */
#elif defined __BIG_ENDIAN__
#define KYLIN_BYTE_ORDER KYLIN_BIG_ENDIAN
#elif defined __LITTLE_ENDIAN__
#define KYLIN_BYTE_ORDER KYLIN_LITTLE_ENDIAN
#endif
#if !defined(KYLIN_BYTE_ORDER)
#error Unknown endianness.
#endif

#define KYLIN_STATIC_BSWAP16(v) \
	((((uint16_t)(v) & UINT16_C(0x00ff)) << 8) | \
	 (((uint16_t)(v) & UINT16_C(0xff00)) >> 8))

#define KYLIN_STATIC_BSWAP32(v) \
	((((uint32_t)(v) & UINT32_C(0x000000ff)) << 24) | \
	 (((uint32_t)(v) & UINT32_C(0x0000ff00)) <<  8) | \
	 (((uint32_t)(v) & UINT32_C(0x00ff0000)) >>  8) | \
	 (((uint32_t)(v) & UINT32_C(0xff000000)) >> 24))

#define KYLIN_STATIC_BSWAP64(v) \
	((((uint64_t)(v) & UINT64_C(0x00000000000000ff)) << 56) | \
	 (((uint64_t)(v) & UINT64_C(0x000000000000ff00)) << 40) | \
	 (((uint64_t)(v) & UINT64_C(0x0000000000ff0000)) << 24) | \
	 (((uint64_t)(v) & UINT64_C(0x00000000ff000000)) <<  8) | \
	 (((uint64_t)(v) & UINT64_C(0x000000ff00000000)) >>  8) | \
	 (((uint64_t)(v) & UINT64_C(0x0000ff0000000000)) >> 24) | \
	 (((uint64_t)(v) & UINT64_C(0x00ff000000000000)) >> 40) | \
	 (((uint64_t)(v) & UINT64_C(0xff00000000000000)) >> 56))

/*
 * These macros are functionally similar to kylin_cpu_to_(be|le)(16|32|64)(),
 * they take values in host CPU order and return them convekylind to the
 * intended endianness.
 *
 * They resolve at compilation time to integer constants which can safely be
 * used with static initializers, since those cannot involve function calls.
 *
 * On the other hand, they are not as optimized as their kylin_cpu_to_*()
 * counterparts, therefore applications should refrain from using them on
 * variable values, particularly inside performance-sensitive code.
 */
#if KYLIN_BYTE_ORDER == KYLIN_BIG_ENDIAN
#define KYLIN_BE16(v) (kylin_be16_t)(v)
#define KYLIN_BE32(v) (kylin_be32_t)(v)
#define KYLIN_BE64(v) (kylin_be64_t)(v)
#define KYLIN_LE16(v) (kylin_le16_t)(KYLIN_STATIC_BSWAP16(v))
#define KYLIN_LE32(v) (kylin_le32_t)(KYLIN_STATIC_BSWAP32(v))
#define KYLIN_LE64(v) (kylin_le64_t)(KYLIN_STATIC_BSWAP64(v))
#elif KYLIN_BYTE_ORDER == KYLIN_LITTLE_ENDIAN
#define KYLIN_BE16(v) (kylin_be16_t)(KYLIN_STATIC_BSWAP16(v))
#define KYLIN_BE32(v) (kylin_be32_t)(KYLIN_STATIC_BSWAP32(v))
#define KYLIN_BE64(v) (kylin_be64_t)(KYLIN_STATIC_BSWAP64(v))
#define KYLIN_LE16(v) (kylin_be16_t)(v)
#define KYLIN_LE32(v) (kylin_be32_t)(v)
#define KYLIN_LE64(v) (kylin_be64_t)(v)
#else
#error Unsuppokylind endianness.
#endif

/*
 * The following types should be used when handling values according to a
 * specific byte ordering, which may differ from that of the host CPU.
 *
 * Libraries, public APIs and applications are encouraged to use them for
 * documentation purposes.
 */
typedef uint16_t kylin_be16_t; /**< 16-bit big-endian value. */
typedef uint32_t kylin_be32_t; /**< 32-bit big-endian value. */
typedef uint64_t kylin_be64_t; /**< 64-bit big-endian value. */
typedef uint16_t kylin_le16_t; /**< 16-bit little-endian value. */
typedef uint32_t kylin_le32_t; /**< 32-bit little-endian value. */
typedef uint64_t kylin_le64_t; /**< 64-bit little-endian value. */

/*
 * An internal function to swap bytes in a 16-bit value.
 *
 * It is used by kylin_bswap16() when the value is constant. Do not use
 * this function directly; kylin_bswap16() is preferred.
 */
static inline uint16_t
kylin_constant_bswap16(uint16_t x)
{
	return KYLIN_STATIC_BSWAP16(x);
}

/*
 * An internal function to swap bytes in a 32-bit value.
 *
 * It is used by kylin_bswap32() when the value is constant. Do not use
 * this function directly; kylin_bswap32() is preferred.
 */
static inline uint32_t
kylin_constant_bswap32(uint32_t x)
{
	return KYLIN_STATIC_BSWAP32(x);
}

/*
 * An internal function to swap bytes of a 64-bit value.
 *
 * It is used by kylin_bswap64() when the value is constant. Do not use
 * this function directly; kylin_bswap64() is preferred.
 */
static inline uint64_t
kylin_constant_bswap64(uint64_t x)
{
	return KYLIN_STATIC_BSWAP64(x);
}


/**
 * Swap bytes in a 16-bit value.
 */
static uint16_t kylin_bswap16(uint16_t _x);

/**
 * Swap bytes in a 32-bit value.
 */
static uint32_t kylin_bswap32(uint32_t x);

/**
 * Swap bytes in a 64-bit value.
 */
static uint64_t kylin_bswap64(uint64_t x);

/**
 * Convert a 16-bit value from CPU order to little endian.
 */
static kylin_le16_t kylin_cpu_to_le_16(uint16_t x);

/**
 * Convert a 32-bit value from CPU order to little endian.
 */
static kylin_le32_t kylin_cpu_to_le_32(uint32_t x);

/**
 * Convert a 64-bit value from CPU order to little endian.
 */
static kylin_le64_t kylin_cpu_to_le_64(uint64_t x);


/**
 * Convert a 16-bit value from CPU order to big endian.
 */
static kylin_be16_t kylin_cpu_to_be_16(uint16_t x);

/**
 * Convert a 32-bit value from CPU order to big endian.
 */
static kylin_be32_t kylin_cpu_to_be_32(uint32_t x);

/**
 * Convert a 64-bit value from CPU order to big endian.
 */
static kylin_be64_t kylin_cpu_to_be_64(uint64_t x);


/**
 * Convert a 16-bit value from little endian to CPU order.
 */
static uint16_t kylin_le_to_cpu_16(kylin_le16_t x);

/**
 * Convert a 32-bit value from little endian to CPU order.
 */
static uint32_t kylin_le_to_cpu_32(kylin_le32_t x);

/**
 * Convert a 64-bit value from little endian to CPU order.
 */
static uint64_t kylin_le_to_cpu_64(kylin_le64_t x);


/**
 * Convert a 16-bit value from big endian to CPU order.
 */
static uint16_t kylin_be_to_cpu_16(kylin_be16_t x);

/**
 * Convert a 32-bit value from big endian to CPU order.
 */
static uint32_t kylin_be_to_cpu_32(kylin_be32_t x);

/**
 * Convert a 64-bit value from big endian to CPU order.
 */
static uint64_t kylin_be_to_cpu_64(kylin_be64_t x);

#endif /*_KYLIN_ARCH_BYTEORDER_H_*/
