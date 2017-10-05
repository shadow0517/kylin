#ifndef _KYLIN_ARCH_ATOMIC_X86_64_H_
#define _KYLIN_ARCH_ATOMIC_X86_64_H_

#include <kylin/include/kylin.h>
//#include <kylin/include/arch/common/kylin_atomic.h>
#include <kylin/include/arch/kylin_atomic.h>

#define	kylin_mb()      _mm_mfence()
#define	kylin_wmb()     _mm_sfence()
#define	kylin_rmb()     _mm_lfence()
#define kylin_smp_mb()  kylin_mb()
#define kylin_smp_wmb() kylin_compiler_barrier()
#define kylin_smp_rmb() kylin_compiler_barrier()
#define kylin_io_mb()   kylin_mb()
#define kylin_io_wmb()  kylin_compiler_barrier()
#define kylin_io_rmb()  kylin_compiler_barrier()

/*------------------------- 16 bit atomic operations -------------------------*/

static inline int kylin_atomic16_cmpset(volatile uint16_t *dst, uint16_t exp, uint16_t src)
{
	uint8_t res;

	asm volatile(MPLOCK
			"cmpxchgw %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */
	return res;
}

static inline int kylin_atomic16_test_and_set(katomic16_t *v)
{
	return kylin_atomic16_cmpset(&v->cnt, 0, 1);
}

static inline void kylin_atomic16_inc(katomic16_t *v)
{
	asm volatile(MPLOCK
			"incw %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline void kylin_atomic16_dec(katomic16_t *v)
{
	asm volatile(MPLOCK
			"decw %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline int kylin_atomic16_inc_and_test(katomic16_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCK
			"incw %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

static inline int kylin_atomic16_dec_and_test(katomic16_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCK
			"decw %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

/*------------------------- 32 bit atomic operations -------------------------*/

static inline int kylin_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src)
{
	uint8_t res;

	asm volatile(MPLOCK
			"cmpxchgl %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */
	return res;
}

static inline int kylin_atomic32_test_and_set(katomic32_t *v)
{
	return kylin_atomic32_cmpset(&v->cnt, 0, 1);
}

static inline void kylin_atomic32_inc(katomic32_t *v)
{
	asm volatile(MPLOCK
			"incl %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline void kylin_atomic32_dec(katomic32_t *v)
{
	asm volatile(MPLOCK
			"decl %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline int kylin_atomic32_inc_and_test(katomic32_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCK
			"incl %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

static inline int kylin_atomic32_dec_and_test(katomic32_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCK
			"decl %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

/*------------------------- 64 bit atomic operations -------------------------*/

static inline int kylin_atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src)
{
	uint8_t res;

	asm volatile(MPLOCK
			"cmpxchgq %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */

	return res;
}

static inline void kylin_atomic64_init(katomic64_t *v)
{
	v->cnt = 0;
}

static inline int64_t kylin_atomic64_read(katomic64_t *v)
{
	return v->cnt;
}

static inline void kylin_atomic64_set(katomic64_t *v, uint64_t new_value)
{
	v->cnt = new_value;
}

static inline void kylin_atomic64_add(katomic64_t *v, uint64_t inc)
{
	asm volatile(MPLOCK
			"addq %[inc], %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: [inc] "ir" (inc),     /* input */
			  "m" (v->cnt)
			);
}

static inline void kylin_atomic64_sub(katomic64_t *v, uint64_t dec)
{
	asm volatile(MPLOCK
			"subq %[dec], %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: [dec] "ir" (dec),     /* input */
			  "m" (v->cnt)
			);
}

static inline void kylin_atomic64_inc(katomic64_t *v)
{
	asm volatile(
			MPLOCK
			"incq %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline void kylin_atomic64_dec(katomic64_t *v)
{
	asm volatile(MPLOCK 
            "decq %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline int64_t kylin_atomic64_add_return(katomic64_t *v, int64_t inc)
{
	uint64_t prev = inc;

	asm volatile(MPLOCK 
            "xaddq %[prev], %[cnt]"
			: [prev] "+r" (prev),   /* output */
			  [cnt] "=m" (v->cnt)
			: "m" (v->cnt)          /* input */
			);
	return prev + inc;
}

static inline int64_t kylin_atomic64_sub_return(katomic64_t *v, int64_t dec)
{
    return kylin_atomic64_add_return(v, -dec);
}

static inline int kylin_atomic64_inc_and_test(katomic64_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCK
			"incq %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt), /* output */
			  [ret] "=qm" (ret)
			);

	return ret != 0;
}

static inline int kylin_atomic64_dec_and_test(katomic64_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCK
			"decq %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

static inline int kylin_atomic64_test_and_set(katomic64_t *v)
{
	return kylin_atomic64_cmpset(&v->cnt, 0, 1);
}

static inline void kylin_atomic64_clear(katomic64_t *v)
{
	v->cnt = 0;
}

#endif /*_KYLIN_ARCH_ATOMIC_X86_64_H_*/
