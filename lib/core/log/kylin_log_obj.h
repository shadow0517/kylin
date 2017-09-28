#ifndef _KYLIN_CORE_LOG_OBJ_
#define _KYLIN_CORE_LOG_OBJ_

#include <kylin/include/kylin_log.h>

/*
 * 日志对象的管理
 */
struct kylin_log {
    uint32_t       id;
    int            logger_count;          /*日志实例个数，-1代表没有任何实例*/
    klog_logger_t  logger[KYLIN_LOG_MAX];
};

extern kerr_t object_insert(klog_t *obj);
extern kerr_t object_remove(uint32_t id);

/*根据id找到相应的日志实例，并返回该日志对象中实例的个数*/
extern int logger_find(uint32_t id, klog_logger_t *);

extern kerr_t object_init(void);
extern void object_fini(void);
#endif /*_KYLIN_CORE_LOG_OBJ_*/
