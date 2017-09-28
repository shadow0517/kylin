#ifndef _KYLIN_ARCH_ATOMIC_H_
#define _KYLIN_ARCH_ATOMIC_H_

/**
 * Compiler barrier.
 *
 * Guarantees that operation reordering does not occur at compile time
 * for operations directly before and after the barrier.
 */
#define	kylin_compiler_barrier() do {		\
	asm volatile ("" : : : "memory");	\
} while(0)

/*------------------------- 16 bit atomic operations -------------------------*/

typedef struct {
	volatile uint16_t cnt; /**< An internal counter value. */
} katomic16_t;

static inline void kylin_atomic16_init(katomic16_t *v)
{
	v->cnt = 0;
}

static inline int16_t kylin_atomic16_read(const katomic16_t *v)
{
	return v->cnt;
}

static inline void kylin_atomic16_set(katomic16_t *v, uint16_t new_value)
{
	v->cnt = new_value;
}

static inline void kylin_atomic16_add(katomic16_t *v, uint16_t inc)
{
	__sync_fetch_and_add(&v->cnt, inc);
}

static inline int16_t kylin_atomic16_add_return(katomic16_t *v, uint16_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

static inline void kylin_atomic16_sub(katomic16_t *v, uint16_t dec)
{
	__sync_fetch_and_sub(&v->cnt, dec);
}

static inline int16_t kylin_atomic16_sub_return(katomic16_t *v, uint16_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}

static inline void kylin_atomic16_clear(katomic16_t *v)
{
	v->cnt = 0;
}

static inline void kylin_atomic16_inc(katomic16_t *v);
static inline void kylin_atomic16_dec(katomic16_t *v);
static inline int kylin_atomic16_inc_and_test(katomic16_t *v);
static inline int kylin_atomic16_dec_and_test(katomic16_t *v);
static inline int kylin_atomic16_test_and_set(katomic16_t *v);
static inline int kylin_atomic16_cmpset(volatile uint16_t *dst, uint16_t exp, uint16_t src);

/*------------------------- 32 bit atomic operations -------------------------*/

typedef struct {
	volatile uint32_t cnt; /**< An internal counter value. */
} katomic32_t;

static inline void kylin_atomic32_init(katomic32_t *v)
{
	v->cnt = 0;
}

static inline int32_t kylin_atomic32_read(const katomic32_t *v)
{
	return v->cnt;
}

static inline void kylin_atomic32_set(katomic32_t *v, uint32_t new_value)
{
	v->cnt = new_value;
}

static inline void kylin_atomic32_add(katomic32_t *v, uint32_t inc)
{
	__sync_fetch_and_add(&v->cnt, inc);
}

static inline void kylin_atomic32_sub(katomic32_t *v, uint32_t dec)
{
	__sync_fetch_and_sub(&v->cnt, dec);
}

static inline int32_t kylin_atomic32_add_return(katomic32_t *v, uint32_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

static inline int32_t kylin_atomic32_sub_return(katomic32_t *v, uint32_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}

static inline void kylin_atomic32_clear(katomic32_t *v)
{
	v->cnt = 0;
}

static inline void kylin_atomic32_inc(katomic32_t *v);
static inline void kylin_atomic32_dec(katomic32_t *v);
static inline int kylin_atomic32_inc_and_test(katomic32_t *v);
static inline int kylin_atomic32_dec_and_test(katomic32_t *v);
static inline int kylin_atomic32_test_and_set(katomic32_t *v);
static inline int kylin_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src);

/*------------------------- 64 bit atomic operations -------------------------*/

typedef struct {
	volatile uint64_t cnt;  /**< Internal counter value. */
} katomic64_t;

static inline void kylin_atomic64_init(katomic64_t *v);
static inline int64_t kylin_atomic64_read(katomic64_t *v);
static inline void kylin_atomic64_set(katomic64_t *v, uint64_t new_value);
static inline void kylin_atomic64_add(katomic64_t *v, uint64_t inc);
static inline void kylin_atomic64_sub(katomic64_t *v, uint64_t dec);
static inline int64_t kylin_atomic64_add_return(katomic64_t *v, uint64_t inc);
static inline int64_t kylin_atomic64_sub_return(katomic64_t *v, uint64_t dec);
static inline void kylin_atomic64_inc(katomic64_t *v);
static inline void kylin_atomic64_dec(katomic64_t *v);
static inline int kylin_atomic64_inc_and_test(katomic64_t *v);
static inline int kylin_atomic64_dec_and_test(katomic64_t *v);
static inline int kylin_atomic64_test_and_set(katomic64_t *v);
static inline int kylin_atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src);

#endif /*_KYLIN_ARCH_ATOMIC_H_*/
