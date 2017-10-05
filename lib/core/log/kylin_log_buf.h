#ifndef _KYLIN_CORE_LOG_BUF_H_
#define _KYLIN_CORE_LOG_BUF_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

/*
 * 日志缓冲区的管理
 * 为每一种日志类型创建一个缓冲区
 */

typedef struct kylin_log_buffer
{
    uint32_t             obj_id; /*日志对象标识符*/
    uint64_t             timestamp;
    klog_level_t         log_level;
    char                 data[KYLIN_LOG_DATA_SIZE];
} klog_buffer_t;

extern kerr_t buffer_enqueue(klog_buffer_t *);

extern kerr_t buffer_init(void);
extern void buffer_fini(void);

#endif /*_KYLIN_CORE_LOG_BUF_H_*/
