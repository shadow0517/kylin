#include <kylin/include/kylin.h>
#include <kylin/include/kylin_timer.h>

#include <kylin/lib/core/timer/kylin_timer_source.h>

static uint64_t __tsc_get_cycle(void);
static uint64_t __hpet_get_cycle(void);
static uint64_t __soft_get_cycle(void);

ktimer_source_t timer_source[KTIMER_SRC_MAX] = {
    {
        .src = KTIMER_SRC_TSC,
        .get_cycle = __tsc_get_cycle
    },
    {
        .src = KTIMER_SRC_HPET,
        .get_cycle = __hpet_get_cycle
    },
    {
        .src = KTIMER_SRC_SOFT,
        .get_cycle = __soft_get_cycle
    }
};

uint64_t __tsc_get_cycle(void)
{
    return 0;
}

uint64_t __hpet_get_cycle(void)
{
    return 0;
}

uint64_t __soft_get_cycle(void)
{
    return 0;
}

kerr_t kylin_timer_source_init(void)
{
    return KYLIN_ERROR_OK;
}

void kylin_timer_source_fini(void)
{
    return;
}

