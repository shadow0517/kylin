#include <kylin/include/kylin.h>
#include <kylin/include/kylin_thread.h>
#include <kylin/include/math/kylin_rb.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/thread/kylin_thread.h>

static krb_t *thread_rb = NULL;

kerr_t kylin_thread_insert(kthread_t *thread)
{
    if(kylin_rb_insert(thread_rb, thread) != NULL)
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

void kylin_thread_remove(kthread_t *thread)
{
    kylin_rb_remove(thread_rb, thread);
}

kthread_t *kylin_thread_unlink(kthread_t *thread)
{
    krb_node_t *node = NULL;

    node = kylin_rb_unlink(thread_rb, thread);
    if(node != NULL)
        return (kthread_t *)kylin_rb_val(thread_rb, node);

    return NULL;
}

kthread_t *kylin_thread_find(pthread_t tid)
{
    kthread_t  cmp;

    memset(&cmp, 0, sizeof(kthread_t));
    cmp.thread_id = tid;

    return (kthread_t *)kylin_rb_find(thread_rb, &cmp);
}

kthread_t *kylin_thread_find_by_name(char *name)
{
    kthread_t *thread = NULL;

    KYLIN_THREAD_FOREACH(thread) {
        if(strncmp(thread->name, name, KYLIN_NAME_LENGTH) == 0)
            return thread;
    }

    return NULL;
}

kthread_t *kylin_thread_get_first(void)
{
    krb_node_t *node = NULL;

    node = kylin_rb_min(thread_rb);
    if(!node)
        return NULL;

    return (kthread_t *)kylin_rb_val(thread_rb, node);
}

kthread_t *kylin_thread_get_next(kthread_t *thread)
{
    krb_node_t *node = NULL;

    node = kylin_rb_next(thread_rb, thread);
    if(!node)
        return NULL;

    return (kthread_t *)kylin_rb_val(thread_rb, node);
}

static int __thread_compare(const void *node1, const void *node2)
{
    const kthread_t *t1 = node1;
    const kthread_t *t2 = node2;

    if(t1->thread_id > t2->thread_id)
        return 1;
    if(t1->thread_id < t2->thread_id)
        return -1;
    return 0;
}

kerr_t kylin_thread_init(void)
{
    krb_opts_t thread_opts = {
        .name      = "thread_rb",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kthread_t),
        .compare   = __thread_compare,
        .allocator = {
            .val_ctor   = NULL, /*值所在的内存由调用者分配*/
            .val_dtor   = NULL,
            .node_ctor  = NULL,
            .node_dtor  = NULL,
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    thread_rb = kylin_rb_create(&thread_opts);
    if(!thread_rb)
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

void kylin_thread_fini(void)
{
    kthread_t *thread = NULL;

    if(thread_rb) {
        KYLIN_THREAD_FOREACH(thread) 
            kylin_thread_terminate(thread);
        KYLIN_THREAD_FOREACH(thread) 
            kylin_thread_stop(thread);
        kylin_rb_destroy(thread_rb);
    }

    return;
}


