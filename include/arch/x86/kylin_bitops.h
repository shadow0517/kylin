#ifndef _KYLIN_ARCH_BITOPS_X86_64_H_
#define _KYLIN_ARCH_BITOPS_X86_64_H_

#include <kylin/include/kylin.h>
//#include <kylin/include/arch/common/kylin_bitops.h>
#include <kylin/include/arch/kylin_bitops.h>

/*
 * set_bit - Atomically set a bit in memory
 */
static inline void kylin_set_bit(kword_t nr, volatile kword_t *addr)
{
    if (IS_CONSTANT(nr)) {
        asm volatile(MPLOCK "orb %1,%0"
                : "+m" (*(volatile long *)(addr + (nr >> 3)))
                : "iq" ((uint8_t)(1 << ((nr) & 7)))
                : "memory");
    } else {
        asm volatile(MPLOCK "bts %1,%0"
                : "+m" (*(volatile long *)addr) 
                : "Jr" (nr) 
                : "memory");
    }
}

/*
 * clear_bit - Clears a bit in memory
 */
static inline void kylin_clear_bit(kword_t nr, volatile kword_t *addr)
{
    if (IS_CONSTANT(nr)) {
        asm volatile(MPLOCK "andb %1,%0"
                : "+m" (*(volatile long *) (addr + (nr>>3)))
                : "iq" ((uint8_t)~(1 << ((nr) & 7))));
    } else {
        asm volatile(MPLOCK "btr %1,%0"
                : "+m" (*(volatile long *)addr)
                : "Jr" (nr));
    }
}

/*
 * change_bit - Toggle a bit in memory
 */
static inline void kylin_change_bit(kword_t nr, volatile kword_t *addr)
{
    if (IS_CONSTANT(nr)) {
        asm volatile(MPLOCK "xorb %1,%0"
                : "+m" (*(volatile long *) (addr + (nr>>3)))
                : "iq" ((uint8_t)(1 << ((nr) & 7))));
    } else {
        asm volatile(MPLOCK "btc %1,%0"
                : "+m" (*(volatile long *)addr)
                : "Jr" (nr));
    }
}

/*
 * test_and_set_bit - Set a bit and return its old value
 */
static inline int kylin_test_and_set_bit(kword_t nr, volatile kword_t *addr)
{
    int oldbit;

    asm volatile(MPLOCK "bts %2,%1\n\t"
            "sbb %0,%0" 
            : "=r" (oldbit), "+m" (*(volatile long *)addr) 
            : "Jr" (nr) 
            : "memory");

    return oldbit;
}

/*
 * test_and_clear_bit - Clear a bit and return its old value
 */
static inline int kylin_test_and_clear_bit(kword_t nr, volatile kword_t *addr)
{
    int oldbit;

    asm volatile(MPLOCK "btr %2,%1\n\t"
            "sbb %0,%0"
            : "=r" (oldbit), "+m" (*(volatile long *)addr) 
            : "Jr" (nr) 
            : "memory");

    return oldbit;
}

/*
 * test_and_change_bit - Change a bit and return its old value
 */
static inline int kylin_test_and_change_bit(kword_t nr, volatile kword_t *addr)
{
    int oldbit;

    asm volatile(MPLOCK "btc %2,%1\n\t"
            "sbb %0,%0"
            : "=r" (oldbit), "+m" (*(volatile long *)addr) 
            : "Jr" (nr) 
            : "memory");

    return oldbit;
}

/*
 * __ffs - find first set bit in word
 */
static inline kword_t kylin_ffs(kword_t word)
{
    asm("rep; bsf %1,%0"
            : "=r" (word)
            : "rm" (word));
    return word;
}

/*
 * ffz - find first zero bit in word
 */
static inline kword_t kylin_ffz(kword_t word)
{
    asm("rep; bsf %1,%0"
            : "=r" (word)
            : "r" (~word));
    return word;
}

/*
 * __fls: find last set bit in word
 */
static inline kword_t kylin_fls(kword_t word)
{
    asm("bsr %1,%0"
            : "=r" (word)
            : "rm" (word));
    return word;
}

#endif /*_KYLIN_ARCH_ATOMIC_X86_64_H_*/
