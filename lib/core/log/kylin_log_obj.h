#ifndef _KYLIN_CORE_LOG_OBJ_H_
#define _KYLIN_CORE_LOG_OBJ_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>
#include <kylin/include/utils/kylin_error.h>

/*
 * 日志对象的管理
 */
struct kylin_log {
    uint32_t       id;
    klog_logger_t  logger[KYLIN_LOG_MAX];
};

extern kerr_t object_insert(klog_t *obj);
extern kerr_t object_remove(uint32_t id);

/*根据id找到相应的日志对象*/
extern klog_t *object_find(uint32_t id);

extern kerr_t object_init(void);
extern void object_fini(void);

#endif /*_KYLIN_CORE_LOG_OBJ_H_*/
