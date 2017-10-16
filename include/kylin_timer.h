#ifndef _KYLIN_TIMER_H_
#define _KYLIN_TIMER_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

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
    KTIMER_TYPE_SINGLE,          /*非阻塞，单次执行*/
    KTIMER_TYPE_PERIODICAL,      /*非阻塞，循环执行*/
    KTIMER_TYPE_BLK_SINGLE,      /*阻塞，单次执行*/
    KTIMER_TYPE_BLK_PERIODICAL   /*阻塞，循环执行*/
} ktimer_type_t;

typedef enum {
    KTIMER_STATE_INIT,
    KTIMER_STATE_STOP,
    KTIMER_STATE_PENDING,
    KTIMER_STATE_RUNNING
} ktimer_state_t;

typedef void (*ktimer_exec_t)(ktimer_t *, void *) 
    __kylin_nonnull((1));

extern ktimer_t *kylin_timer_create(uint64_t, 
        ktimer_src_t, ktimer_type_t, ktimer_exec_t, void *, int) 
    __kylin_nonnull((4));
extern void kylin_timer_destroy(ktimer_t *) 
    __kylin_nonnull((1));

extern kerr_t kylin_timer_start(ktimer_t *) 
    __kylin_nonnull((1));
extern kerr_t kylin_timer_start_sync(ktimer_t *) 
    __kylin_nonnull((1));

extern kerr_t kylin_timer_stop(ktimer_t *) 
    __kylin_nonnull((1));
extern kerr_t kylin_timer_stop_sync(ktimer_t *) 
    __kylin_nonnull((1));

extern kbool_t kylin_timer_is_pending(ktimer_t *) 
    __kylin_nonnull((1));

#endif /*_KYLIN_TIMER_H_*/

