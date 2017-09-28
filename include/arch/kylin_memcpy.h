#ifndef _KYLIN_ARCH_MEMCPY_H_
#define _KYLIN_ARCH_MEMCPY_H_

/*
 * Functions for vectorised implementation of memcpy().
 */

/**
 * Copy 16 bytes from one location to another using optimised
 * instructions. The locations should not overlap.
 */
static inline void kylin_mov16(uint8_t *dst, const uint8_t *src);

/**
 * Copy 32 bytes from one location to another using optimised
 * instructions. The locations should not overlap.
 */
static inline void kylin_mov32(uint8_t *dst, const uint8_t *src);

/**
 * Copy 64 bytes from one location to another using optimised
 * instructions. The locations should not overlap.
 */
static inline void kylin_mov64(uint8_t *dst, const uint8_t *src);

/**
 * Copy 128 bytes from one location to another using optimised
 * instructions. The locations should not overlap.
 */
static inline void kylin_mov128(uint8_t *dst, const uint8_t *src);

/**
 * Copy 256 bytes from one location to another using optimised
 * instructions. The locations should not overlap.
 */
static inline void kylin_mov256(uint8_t *dst, const uint8_t *src);

/**
 * Copy bytes from one location to another. The locations must not overlap.
 * @return
 *   Pointer to the destination data.
 */
static void * kylin_memcpy(void *dst, const void *src, size_t n);

#endif /*_KYLIN_ARCH_MEMCPY_H_*/
