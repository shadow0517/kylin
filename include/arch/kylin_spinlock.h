#ifndef _KYLIN_ARCH_SPINLOCK_H_
#define _KYLIN_ARCH_SPINLOCK_H_

typedef struct {
	volatile int locked; /**< lock status 0 = unlocked, 1 = locked */
} kspinlock_t;

static inline void kylin_spinlock_init(kspinlock_t *sl)
{
	sl->locked = 0;
}

static inline int kylin_spinlock_is_locked(kspinlock_t *sl)
{
	return sl->locked;
}

static inline void kylin_spinlock_lock(kspinlock_t *sl);
static inline void kylin_spinlock_unlock(kspinlock_t *sl);
static inline int kylin_spinlock_trylock(kspinlock_t *sl);

#endif /*_KYLIN_ARCH_SPINLOCK_H_*/
