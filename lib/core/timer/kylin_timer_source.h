#ifndef _KYLIN_CORE_TIMER_SOURCE_H_
#define _KYLIN_CORE_TIMER_SOURCE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_timer.h>

typedef struct {
    ktimer_src_t src;
    uint64_t (*get_cycle)(void);
} ktimer_source_t;

extern ktimer_source_t timersource[KTIMER_SRC_MAX];

extern kerr_t kylin_timer_source_init(void);
extern void kylin_timer_source_fini(void);

#endif /*_KYLIN_CORE_TIMER_SOURCE_H_*/
