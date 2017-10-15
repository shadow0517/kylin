#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>
#include <kylin/include/math/kylin_ring.h>
#include <kylin/include/utils/kylin_object.h>

#include <kylin/lib/core/log/kylin_log_obj.h>
#include <kylin/lib/core/log/kylin_log_buf.h>

static pthread_t log_tid;
static kring_t  *log_ring[KYLIN_LOG_MAX] = {NULL};

/*标记当前时刻有哪些日志缓冲区有值*/
static kbool_t need_log[KYLIN_LOG_MAX] = {KYLIN_FALSE};

static kring_opts_t log_ring_opts = {
    .name      = "log_buffer",
    .cap       = 1024,
    .val_type  = KOBJ_OTHERS,
    .val_size  = sizeof(klog_buffer_t),
    .allocator = KRING_OPTS_ALLOCATOR_VAL(malloc)
};

kerr_t buffer_enqueue(klog_buffer_t *buf)
{
    klog_t *obj = NULL;

    obj = object_find(buf->obj_id);
    if(!obj)
        return KYLIN_ERROR_NOENT;

    for(int i = 0; i < KYLIN_LOG_MAX; i++) {
        if(obj->logger[i].type != i ||
                buf->log_level > obj->logger[i].level)
            continue;
        kylin_ring_enqueue(log_ring[i], buf);
        need_log[i] = KYLIN_TRUE;
    }

    return KYLIN_ERROR_OK;
}

static void __buffer_dequeue(uint32_t type)
{
    klog_t        *obj = NULL;
    klog_buffer_t *buf = NULL;

    buf = (klog_buffer_t *)kylin_ring_dequeue(log_ring[type]);
    if(!buf) {
        need_log[type] = KYLIN_FALSE;
        return;
    }

    obj = object_find(buf->obj_id);
    if(!obj) {
        free(buf);
        return;
    }

    if(obj->logger[type].print)
        obj->logger[type].print(buf->timestamp, buf->log_level, buf->data);

    free(buf);

    buf = (klog_buffer_t *)kylin_ring_peek(log_ring[type]);
    if(!buf) 
        need_log[type] = KYLIN_FALSE;
}

static void *__buffer_process(void *arg __kylin_unused)
{
    kbool_t need_sleep = KYLIN_TRUE;

    while(1) {
        for(int i = 0; i < KYLIN_LOG_MAX; i++) {
            if(need_log[i]) {
                need_sleep = KYLIN_FALSE;
                __buffer_dequeue(i);
            }
        }

        if(need_sleep)
            sleep(1);
        need_sleep = KYLIN_TRUE;
    }

    return NULL;
}

kerr_t buffer_init(void)
{
    for(int i = 0; i < KYLIN_LOG_MAX; i++) {
        log_ring[i] = kylin_ring_create(&log_ring_opts); 
        if(!log_ring[i])
            return KYLIN_ERROR_NOMEM;
    }

    return pthread_create(&log_tid, NULL, __buffer_process, NULL);
}

void buffer_fini(void)
{
    pthread_join(log_tid, NULL);

    for(int i = 0; i < KYLIN_LOG_MAX; i++) {
        kylin_ring_destroy(log_ring[i]);
    }
}

