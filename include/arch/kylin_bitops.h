#ifndef _KYLIN_ARCH_BITOPS_H_
#define _KYLIN_ARCH_BITOPS_H_

/*
 * set_bit - Atomically set a bit in memory
 */
static inline void kylin_set_bit(kword_t nr, volatile kword_t *addr);

/*
 * clear_bit - Clears a bit in memory
 */
static inline void kylin_clear_bit(kword_t nr, volatile kword_t *addr);

/*
 * change_bit - Toggle a bit in memory
 */
static inline void kylin_change_bit(kword_t nr, volatile kword_t *addr);

/*
 * test_and_set_bit - Set a bit and return its old value
 */
static inline int kylin_test_and_set_bit(kword_t nr, volatile kword_t *addr);

/*
 * test_and_clear_bit - Clear a bit and return its old value
 */
static inline int kylin_test_and_clear_bit(kword_t nr, volatile kword_t *addr);

/*
 * test_and_change_bit - Change a bit and return its old value
 */
static inline int kylin_test_and_change_bit(kword_t nr, volatile kword_t *addr);

/*
 * ffs - find first set bit in word
 */
static inline kword_t kylin_ffs(kword_t word);

/*
 * ffz - find first zero bit in word
 */
static inline kword_t kylin_ffz(kword_t word);

/*
 * fls: find last set bit in word
 */
static inline kword_t kylin_fls(kword_t word);

/*
 * roundup: find roundup power of two in word
 */
static inline kword_t kylin_roundup_pow_of_two(kword_t word)
{
    return 1UL << kylin_fls(word - 1);
}

/*
 * rounddown: find round down power of two in word
 */
static inline kword_t kylin_rounddown_pow_of_two(kword_t word)
{
    return 1UL << (kylin_fls(word) - 1);
}

#endif /*_KYLIN_ARCH_BITOPS_H_*/
