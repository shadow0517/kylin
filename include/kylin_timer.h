#ifndef _KYLIN_TIMER_H_
#define _KYLIN_TIMER_H_

/*
 * 定时器
 */
struct kylin_timer;
typedef struct kylin_timer ktimer_t;

typedef enum {
    KTIMER_SRC_TSC   = 0x00,
    KTIMER_SRC_HPET,
    KTIMER_SRC_SOFT,
    KTIMER_SRC_MAX
} ktimer_src_t;

typedef enum {
    KTIMER_TYPE_SINGLE,
    KTIMER_TYPE_PERIODICAL
} ktimer_type_t;

typedef enum {
    KTIMER_STATE_INIT,
    KTIMER_STATE_STOP,
    KTIMER_STATE_PENDING,
    KTIMER_STATE_RUNNING,
    KTIMER_STATE_CONFIG
} ktimer_state_t;

typedef void (*ktimer_exec_t)(ktimer_t *, void *);

extern ktimer_t *kylin_timer_create(uint64_t, 
        ktimer_src_t, ktimer_type_t, ktimer_exec_t, void *, int);
extern void kylin_timer_destroy(ktimer_t *);

extern kerr_t kylin_timer_start(ktimer_t *);
extern kerr_t kylin_timer_start_sync(ktimer_t *);

extern kerr_t kylin_timer_stop(ktimer_t *);
extern kerr_t kylin_timer_stop_sync(ktimer_t *);

extern kbool_t kylin_timer_is_pending(ktimer_t *);

#endif /*_KYLIN_TIMER_H_*/

