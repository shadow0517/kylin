#ifndef _KYLIN_LOG_H_
#define _KYLIN_LOG_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

#define _LOG_FORMAT_         __kylin_format_print(2, 3)
#define KYLIN_LOG_DATA_SIZE  2048  /*每一条日志能记录的最大长度*/

/*日志对象*/
struct kylin_log;
typedef struct kylin_log klog_t;

/*
 * 对每一个进程而言，日志模块提供一个指向默认日志类型的指针
 * 日志类型对应的日志等级都为KYLIN_LOG_INFO
 * 指针内容只能由用户模块使用，而不能修改
 * 并且用户模块不可以改动指针的指向，这会造成不可预计的后果
 */
extern klog_t *klog;

/*
 * 日志模块支持3种（console, file, system）默认类型
 * 同时给用户提供其他日志类型的注册接口
 * 最多可以支持64种日志类型
 */
typedef enum kylin_log_type {
    KYLIN_LOG_CONSOLE                    = 0UL,
    KYLIN_LOG_FILE                       = 1UL,
    KYLIN_LOG_SYSTEM                     = 2UL,
    KYLIN_LOG_UNIX_SOCK                  = 3UL, /*ipc, user defined*/
    KYLIN_LOG_MAX                        = 4UL,
}klog_type_t;

/*
 *日志等级
 */
typedef enum kylin_log_level {
    KYLIN_LOG_EMERG  = 0x00,
    KYLIN_LOG_ALERT  = 0x01,
    KYLIN_LOG_CRIT   = 0x02,
    KYLIN_LOG_ERROR  = 0x03,
    KYLIN_LOG_WARN   = 0x04,
    KYLIN_LOG_NOTICE = 0x05,
    KYLIN_LOG_INFO   = 0x06,
    KYLIN_LOG_DEBUG  = 0x07,
}klog_level_t;

static __kylin_inline char *kylin_log_level_string(klog_level_t level)
{
    switch(level) {
        case KYLIN_LOG_EMERG:
            return "LOG_EMERG";
        case KYLIN_LOG_ALERT:
            return "LOG_ALERT";
        case KYLIN_LOG_CRIT:
            return "LOG_CRIT";
        case KYLIN_LOG_ERROR:
            return "LOG_ERROR";
        case KYLIN_LOG_WARN:
            return "LOG_WARN";
        case KYLIN_LOG_NOTICE:
            return "LOG_NOTICE";
        case KYLIN_LOG_INFO:
            return "LOG_INFO";
        case KYLIN_LOG_DEBUG:
            return "LOG_DEBUG";
        default:
            return "LOG_UNKNOWN";
    }
}

/*
 * 日志打印函数
 * 用户自定义日志类型需要实现
 */
typedef void (*kylin_log_print)(uint64_t timestamp, klog_level_t level, const char *buf); 

/*
 * 日志实例，对应日志等级
 * 对于默认日志类型，日志模块提供已注册好的日志实例
 */
typedef struct kylin_logger {
    klog_type_t       type;
    klog_level_t      level;
    kylin_log_print   print;
    void             *priv;
    kerr_t (*ctor)(void *);
    kerr_t (*dtor)(void *);
}klog_logger_t;

extern klog_t *kylin_log_create(klog_logger_t logger[], size_t num) 
    __kylin_nonnull((1));
extern void kylin_log_destroy(klog_t *) 
    __kylin_nonnull((1));

extern klog_t *kylin_log_dup(const klog_t *) 
    __kylin_nonnull((1));              /*日志对象副本*/
extern kerr_t kylin_log_expand(klog_t *, klog_logger_t *) 
    __kylin_nonnull((1, 2));           /*日志对象添加实例*/
extern kerr_t kylin_log_reduce(klog_t *, klog_type_t) 
    __kylin_nonnull((1));              /*日志对象缩减实例*/

extern klog_logger_t *kylin_log_logger_get(klog_t *, klog_type_t) 
    __kylin_nonnull((1));
extern klog_level_t kylin_log_level_get(klog_t *, klog_type_t) 
    __kylin_nonnull((1));

extern kerr_t kylin_log_logger_set(klog_t *, klog_type_t, klog_logger_t *) 
    __kylin_nonnull((1, 3));
extern kerr_t kylin_log_level_set(klog_t *, klog_type_t, klog_level_t) 
    __kylin_nonnull((1));

extern void kylin_log_debug(const klog_t *, const char *, ...)  
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;
extern void kylin_log_info(const klog_t *, const char *, ...)   
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;
extern void kylin_log_notice(const klog_t *, const char *, ...) 
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;
extern void kylin_log_warn(const klog_t *, const char *, ...)   
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;
extern void kylin_log_error(const klog_t *, const char *, ...)  
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;
extern void kylin_log_crit(const klog_t *, const char *, ...)   
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;
extern void kylin_log_alert(const klog_t *, const char *, ...)  
    __kylin_nonnull((1, 2)) _LOG_FORMAT_;

#endif /*_KYLIN_LOG_H_*/
