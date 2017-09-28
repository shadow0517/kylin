#ifndef _KYLIN_ARCH_RWLOCK_H_
#define _KYLIN_ARCH_RWLOCK_H_

#include <kylin/include/arch/kylin_atomic.h>
#include <kylin/include/arch/kylin_pause.h>

typedef struct {
	volatile uint32_t cnt; /**< 0xFFFFFFFF when W lock held, others when R locks held. */
} krwlock_t;

static inline void kylin_rwlock_init(krwlock_t *rwl)
{
	rwl->cnt = 0;
}

static inline void kylin_rwlock_read_lock(krwlock_t *rwl)
{
	uint32_t x;
	int success = 0;

	while (success == 0) {
		x = rwl->cnt;
		/* write lock is held */
		if (x == 0xFFFFFFFF) {
			kylin_pause();
			continue;
		}
		success = kylin_atomic32_cmpset(&rwl->cnt, x, x + 1);
	}
}

static inline void kylin_rwlock_read_unlock(krwlock_t *rwl)
{
	kylin_atomic32_dec((katomic32_t *)&rwl->cnt);
}

static inline void kylin_rwlock_write_lock(krwlock_t *rwl)
{
	uint32_t x;
	int success = 0;

	while (success == 0) {
		x = rwl->cnt;
		/* a lock is held */
		if (x != 0) {
			kylin_pause();
			continue;
		}
		success = kylin_atomic32_cmpset(&rwl->cnt, 0, 0xFFFFFFFF);
	}
}

static inline void kylin_rwlock_write_unlock(krwlock_t *rwl)
{
	kylin_atomic32_inc((katomic32_t *)&rwl->cnt);
}

#endif /*_KYLIN_ARCH_RWLOCK_H_*/
