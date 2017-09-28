#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

#include <kylin/lib/core/log/kylin_log.h>
#include <kylin/lib/core/log/kylin_log_buf.h>
#include <kylin/lib/core/log/kylin_log_obj.h>
#include <kylin/lib/core/log/plugin/kylin_log_console.h>
#include <kylin/lib/core/log/plugin/kylin_log_file.h>
#include <kylin/lib/core/log/plugin/kylin_log_system.h>

const klog_t *klog = NULL;

#define KYLIN_LOG_NORMAL(level)                                \
{                                                              \
    va_list args;                                              \
    klog_buffer_t buf;                                         \
                                                               \
    memset(&buf, 0, sizeof(klog_buffer_t));                    \
                                                               \
    va_start(args, fmt);                                       \
                                                               \
    buf.logger_id = obj->id;                                   \
    buf.timestamp = time(NULL);                                \
    buf.log_level = level;                                     \
    vsnprintf(buf.data, KYLIN_LOG_DATA_SIZE, fmt, args);       \
                                                               \
    buffer_enqueue(&buf);                                      \
                                                               \
    va_end(args);                                              \
}

klog_t *kylin_log_create(klog_logger_t logger[], size_t num)
{

}

void kylin_log_destroy(klog_t *)
{

}

klog_t *kylin_log_dup(const klog_t *)
{

}

kerr_t kylin_log_expand(klog_t *, klog_logger_t)
{

}

kerr_t kylin_log_reduce(klog_t *, klog_type_t)
{

}

klog_logger_t *kylin_log_logger_get(klog_t *, klog_type_t)
{

}

klog_level_t kylin_log_level_get(klog_t *, klog_type_t)
{

}

kerr_t kylin_log_logger_set(klog_t *, klog_type_t, klog_logger_t *)
{

}

kerr_t kylin_log_level_set(klog_t *, klog_type_t, klog_level_t)
{

}

void kylin_log_debug(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_DEBUG);
}

void kylin_log_info(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_INFO);
}

void kylin_log_notice(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_NOTICE);
}

void kylin_log_warn(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_WARN);
}

void kylin_log_error(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_ERROR);
}

void kylin_log_crit(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_CRIT);
}

void kylin_log_alert(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_NORMAL(KYLIN_LOG_ALERT);
}

kerr_t kylin_log_init(void) 
{

}

void kylin_log_fini(void)
{

}

