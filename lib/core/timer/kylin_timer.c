#include <kylin/include/kylin.h>
#include <kylin/include/kylin_timer.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>
#include <kylin/include/math/kylin_rb.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/timer/kylin_timer.h>
#include <kylin/lib/core/timer/kylin_timer_source.h>

struct kylin_timer
{
	uint64_t        expire;   /*Time when timer expire.*/
	uint64_t        period;   /*Period of timer (0 if not periodic). */

    ktimer_src_t    src;
    ktimer_type_t   type;
    ktimer_state_t  state;

	ktimer_exec_t   exec;     /*Callback function.*/
    size_t          arg_size;
	kbyte_t         arg[];    /*Argument to callback function.*/
};

static krb_t *timer_rb[KTIMER_SOURCE_MAX];
static kspinlock_t lock[KTIMER_SOURCE_MAX];

ktimer_t *kylin_timer_create(uint64_t expire, 
        ktimer_src_t src, ktimer_type_t type, ktimer_exec_t exec, void *arg, int arg_size)
{
    ktimer_t *timer = NULL;

    timer = malloc(sizeof(ktimer_t) + arg_size);
    if(!timer)
        return NULL;
    memset(timer, 0, sizeof(ktimer_t) + arg_size);

    timer->expire = expire;
    if(type == KTIMER_TYPE_PERIODICAL)
        timer->period = expire;

    timer->src   = src;
    timer->type  = type;
    timer->state = KTIMER_STATE_INIT; 

    timer->exec     = exec;
    timer->arg_size = arg_size;
    memcpy(timer->arg, arg, arg_size);

    return timer;
}

void kylin_timer_destroy(ktimer_t *timer)
{
    free(timer);
}

kerr_t kylin_timer_start(ktimer_t *timer)
{

}

kerr_t kylin_timer_start_sync(ktimer_t *timer)
{

}

kerr_t kylin_timer_stop(ktimer_t *timer)
{

}

kerr_t kylin_timer_stop_sync(ktimer_t *timer)
{

}

kbool_t kylin_timer_is_pending(ktimer_t *timer)
{

}

static void *__kylin_timer_schedule(void *arg __kylin_inline)
{

}

kerr_t kylin_timer_init(void)
{

}

void kylin_timer_fini(void)
{

}

