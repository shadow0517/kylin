#ifndef _KYLIN_ARCH_PREFETCH_H_
#define _KYLIN_ARCH_PREFETCH_H_

/**
 * Prefetch a cache line into all cache levels.
 */
static inline void kylin_prefetch0(const volatile void *p);

/**
 * Prefetch a cache line into all cache levels except the 0th cache level.
 */
static inline void kylin_prefetch1(const volatile void *p);

/**
 * Prefetch a cache line into all cache levels except the 0th and 1th cache levels.
 */
static inline void kylin_prefetch2(const volatile void *p);

/**
 * Prefetch a cache line into all cache levels (non-temporal/transient version)
 */
static inline void kylin_prefetch_non_temporal(const volatile void *p);

#endif /*_KYLIN_ARCH_PREFETCH_H_*/
