#ifndef _KYLIN_THREAD_H_
#define _KYLIN_THREAD_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

struct kylin_thread;
typedef struct kylin_thread kthread_t;

extern kthread_t *kylin_thread_create(char *, void (*exec)(kthread_t *thread), void *, int)
    __THROWNL __nonnull ((1, 2));
extern void kylin_thread_destroy(kthread_t *) 
    __THROWNL __nonnull ((1));

/*启动/终止线程和判断线程是否终止*/
extern kerr_t kylin_thread_start(kthread_t *)
    __THROWNL __nonnull ((1));
extern kerr_t kylin_thread_stop(kthread_t *)
    __THROWNL __nonnull ((1));

extern void kylin_thread_terminate(kthread_t *)
    __THROWNL __nonnull ((1));
extern kbool_t kylin_thread_is_terminated(kthread_t *)
    __THROWNL __nonnull ((1));

/*线程参数的获取和修改*/
extern void *kylin_thread_param(kthread_t *)
    __THROWNL __nonnull ((1));
extern void kylin_thread_param_set(kthread_t *, void *)
    __THROWNL __nonnull ((1, 2));

/*线程名称的获取和修改*/
extern char *kylin_thread_name(kthread_t *)
    __THROWNL __nonnull ((1));
extern void kylin_thread_name_set(kthread_t *, char *)
    __THROWNL __nonnull ((1, 2));

/*线程优先级的获取和修改*/
extern uint8_t kylin_thread_priority(kthread_t *)
    __THROWNL __nonnull ((1));
extern kerr_t kylin_thread_priority_set(kthread_t *, uint8_t)
    __THROWNL __nonnull ((1));

/*线程亲和性的获取和修改*/
extern kerr_t kylin_thread_affinity(kthread_t *thread, kcpuset_t *mask)
    __THROWNL __nonnull ((1, 2));
extern kerr_t kylin_thread_affinity_set(kthread_t *, uint8_t)
    __THROWNL __nonnull ((1));

#endif /*_KYLIN_THREAD_H_*/
