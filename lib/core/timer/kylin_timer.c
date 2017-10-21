#include <kylin/include/kylin.h>
#include <kylin/include/kylin_timer.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>
#include <kylin/include/arch/x86/kylin_pause.h>
#include <kylin/include/math/kylin_rb.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/timer/kylin_timer.h>
#include <kylin/lib/core/timer/kylin_timer_source.h>

struct kylin_timer
{
	uint64_t        expire;   /*Time when timer expire*/
	uint64_t        period;   /*Period of timer (0 if not periodic)*/

    ktimer_src_t    src;
    ktimer_type_t   type;
    ktimer_state_t  state;

	ktimer_exec_t   exec;     /*Callback function*/
    size_t          arg_size;
	kbyte_t         arg[];    /*Argument to callback function*/
};

static krb_t *timer_pending_rb[KTIMER_SRC_MAX];
static kspinlock_t lock[KTIMER_SRC_MAX];

static pthread_t timer_tid;

ktimer_t *kylin_timer_create(uint64_t expire, 
        ktimer_src_t src, ktimer_type_t type, ktimer_exec_t exec, void *arg, int arg_size)
{
    ktimer_t *timer = NULL;

    timer = kylin_malloc(sizeof(ktimer_t) + arg_size);
    if(!timer)
        return NULL;

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
    int src = timer->src;

    while(timer->state == KTIMER_STATE_RUNNING)
        kylin_pause();

    kylin_spinlock_lock(&lock[src]);
    kylin_rb_remove(timer_pending_rb[timer->src], timer);
    kylin_spinlock_unlock(&lock[src]);
}

kerr_t kylin_timer_start(ktimer_t *timer)
{
    uint64_t expire = timer->expire;

    if(timer->state == KTIMER_STATE_INIT)
        timer->expire += timer_source[timer->src].get_cycle();
    else
        return KYLIN_ERROR_NOENT;

    kylin_spinlock_lock(&lock[timer->src]);
    if(!kylin_rb_insert(timer_pending_rb[timer->src], timer)) {
        kylin_spinlock_unlock(&lock[timer->src]);
        timer->expire = expire;
        return KYLIN_ERROR_NOMEM;
    }
    kylin_spinlock_unlock(&lock[timer->src]);

    timer->state = KTIMER_STATE_PENDING;

    return KYLIN_ERROR_OK;
}

kerr_t kylin_timer_start_sync(ktimer_t *timer)
{
    kerr_t ret;

    while((ret = kylin_timer_start(timer)) != KYLIN_ERROR_OK){
        if(ret == KYLIN_ERROR_NOENT)
            return ret;
        kylin_pause();
    }

    return KYLIN_ERROR_OK;
}

kerr_t kylin_timer_stop(ktimer_t *timer)
{
    if(timer->state == KTIMER_STATE_RUNNING)
        return KYLIN_ERROR_NOENT;

    kylin_spinlock_lock(&lock[timer->src]);
    if(!kylin_rb_unlink(timer_pending_rb[timer->src], timer)) {
        kylin_spinlock_unlock(&lock[timer->src]);
        return KYLIN_ERROR_INVAL;
    }
    kylin_spinlock_unlock(&lock[timer->src]);

    timer->state = KTIMER_STATE_STOP;

    return KYLIN_ERROR_OK;
}

kerr_t kylin_timer_stop_sync(ktimer_t *timer)
{
    while(timer->state == KTIMER_STATE_RUNNING)
        kylin_pause();

    return kylin_timer_stop(timer);
}

kbool_t kylin_timer_is_pending(ktimer_t *timer)
{
    return timer->state == KTIMER_STATE_PENDING ? KYLIN_TRUE : KYLIN_FALSE;
}

static void *__kylin_timer_schedule(void *arg __kylin_unused)
{
    uint64_t    cur_timestamp = 0;
    ktimer_t   *cur_timer = NULL;
    krb_node_t *cur_node = NULL;

    for(int i = 0; i < KTIMER_SRC_MAX; i++) {
        cur_timestamp = timer_source[i].get_cycle();

        kylin_spinlock_lock(&lock[i]);
        while((cur_node = kylin_rb_min(timer_pending_rb[i])) != NULL) {
            cur_timer = kylin_rb_val(timer_pending_rb[i], cur_node);
            if(!cur_timer || 
                    cur_timer->expire > cur_timestamp) {
                kylin_spinlock_unlock(&lock[i]);
                break;
            }

            kylin_rb_unlink(timer_pending_rb[i], cur_timer);
            kylin_spinlock_unlock(&lock[i]);

            cur_timer->state = KTIMER_STATE_RUNNING;
            cur_timer->exec(cur_timer, cur_timer->arg);

            if(!cur_timer->period) {
                cur_timer->expire = cur_timer->period + cur_timestamp;
                cur_timer->state  = KTIMER_STATE_PENDING;

                kylin_spinlock_lock(&lock[i]);
                while(!kylin_rb_insert(timer_pending_rb[i], cur_timer))
                    cur_timer->expire++;
                kylin_spinlock_unlock(&lock[i]);

            }
            else
                cur_timer->state  = KTIMER_STATE_STOP;
        }
    }

    return NULL;
}

static int __timer_compare(const void *v1, const void *v2)
{
    const ktimer_t *tim1 = v1;
    const ktimer_t *tim2 = v2;

    if(tim1->expire > tim2->expire)
        return 1;
    if(tim1->expire < tim2->expire)
        return -1;
    return 0;
}

kerr_t kylin_timer_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    krb_opts_t timer_opts = {
        .name = "timer",
        .val_type = KOBJ_OTHERS,
        .val_size = sizeof(ktimer_t),
        .compare = __timer_compare,
        .allocator = KRB_OPTS_ALLOCATOR_NULL
    };

    for(int i = 0; i < KTIMER_SRC_MAX; i++) {
        timer_pending_rb[i] = kylin_rb_create(&timer_opts);
        if(!timer_pending_rb[i])
            return KYLIN_ERROR_NOMEM;
        kylin_spinlock_init(&lock[i]);
    }

    ret = kylin_timer_source_init();
    if(ret != KYLIN_ERROR_OK)
        return ret;

    return pthread_create(&timer_tid, NULL, __kylin_timer_schedule, NULL);
}

void kylin_timer_fini(void)
{
    for(int i = 0; i < KTIMER_SRC_MAX; i++) {
        if(timer_pending_rb[i])
            kylin_rb_destroy(timer_pending_rb[i]);
    }

    kylin_timer_source_fini();

    pthread_join(timer_tid, NULL);
}

