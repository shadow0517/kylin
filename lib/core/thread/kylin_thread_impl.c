#include <kylin/include/kylin.h>
#include <kylin/include/kylin_thread.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/thread/kylin_thread.h>

static void * __thread_proc(void *param) 
{
    kthread_t *thread = param;

    thread->exec(thread);

    return NULL;
}

kthread_t *kylin_thread_create(char *name, 
        void (*exec)(kthread_t *thread), void *param, int param_size)
{
    kthread_t *thread = NULL;

    thread = kylin_malloc(sizeof(kthread_t) + param_size);
    if(!thread)
        return NULL;

    memcpy(thread->param, param, param_size);
    memcpy(thread->name, name, KYLIN_MIN(strlen(name), (KYLIN_NAME_LENGTH - 1)));
    thread->terminated = KYLIN_FALSE;
    thread->param_size = param_size;
    thread->exec       = exec;

    if(kylin_thread_insert(thread) != KYLIN_ERROR_OK) {
        kylin_free(thread);
        return NULL;
    }

    return thread;
}

void kylin_thread_destroy(kthread_t *thread)
{
    thread = kylin_thread_unlink(thread);
    if(!thread)
        return;

    kylin_thread_terminate(thread);
    kylin_thread_stop(thread);

    kylin_free(thread);
}

kerr_t kylin_thread_start(kthread_t *thread)
{
    int ret;

    ret = pthread_create(&thread->thread_id, NULL, __thread_proc, thread);
    if(ret == 0) {
#if defined(__linux__)
        pthread_setname_np(thread->thread_id, thread->name);
#elif defined(__FreeBSD__)
        pthread_set_name_np(thread->thread_id, thread->name);
#endif /*__SYS_LINUX__*/
    }

    return ret;
}

kerr_t kylin_thread_stop(kthread_t *thread)
{
     return pthread_join(thread->thread_id, NULL);
}

void kylin_thread_terminate(kthread_t *thread)
{
    thread->terminated = 1;
}

kbool_t kylin_thread_is_terminated(kthread_t *thread)
{
    return thread->terminated;
}

void *kylin_thread_param(kthread_t *thread)
{
    return thread->param;
}

void kylin_thread_param_set(kthread_t *thread, void *param)
{
    memcpy(thread->param, param, thread->param_size);
}

char *kylin_thread_name(kthread_t *thread)
{
    return thread->name;
}

void kylin_thread_name_set(kthread_t *thread, char *name)
{
    memset(thread->name, 0, KYLIN_NAME_LENGTH);
    memcpy(thread->name, name, KYLIN_MIN(strlen(name), (KYLIN_NAME_LENGTH - 1)));

#if defined(__linux__)
    pthread_setname_np(thread->thread_id, thread->name);
#elif defined(__FreeBSD__)
    pthread_set_name_np(thread->thread_id, thread->name);
#endif /*__SYS_LINUX__*/

    return;
}

uint8_t kylin_thread_priority(kthread_t *thread)
{
    int policy;
    struct sched_param param;

    memset(&param, 0, sizeof(struct sched_param));
    pthread_getschedparam(thread->thread_id, &policy, &param);

    return param.sched_priority;
}

kerr_t kylin_thread_priority_set(kthread_t *thread, uint8_t priority)
{
    struct sched_param param;

    memset(&param, 0, sizeof(struct sched_param));
    param.sched_priority = priority;

    return pthread_setschedparam(thread->thread_id, SCHED_RR, &param);
}

kerr_t kylin_thread_affinity(kthread_t *thread, kcpuset_t *mask)
{
    return pthread_getaffinity_np(thread->thread_id, sizeof(*mask), mask);
}

kerr_t kylin_thread_affinity_set(kthread_t *thread, uint8_t cpuid)
{
    kcpuset_t cpu_mask;

    CPU_ZERO(&cpu_mask);
    CPU_SET(cpuid, &cpu_mask);

    return pthread_setaffinity_np(thread->thread_id, sizeof(cpu_mask), &cpu_mask);
}

