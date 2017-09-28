#ifndef _KYLIN_ARCH_PREFETCH_X86_64_H_
#define _KYLIN_ARCH_PREFETCH_X86_64_H_

#include <kylin/include/kylin.h>
#include <kylin/include/arch/common/kylin_prefetch.h>

static inline void kylin_prefetch0(const volatile void *p)
{
	asm volatile ("prefetcht0 %[p]" : : [p] "m" (*(const volatile char *)p));
}

static inline void kylin_prefetch1(const volatile void *p)
{
	asm volatile ("prefetcht1 %[p]" : : [p] "m" (*(const volatile char *)p));
}

static inline void kylin_prefetch2(const volatile void *p)
{
	asm volatile ("prefetcht2 %[p]" : : [p] "m" (*(const volatile char *)p));
}

static inline void kylin_prefetch_non_temporal(const volatile void *p)
{
	asm volatile ("prefetchnta %[p]" : : [p] "m" (*(const volatile char *)p));
}

#endif /*_KYLIN_ARCH_PREFETCH_X86_64_H_*/
