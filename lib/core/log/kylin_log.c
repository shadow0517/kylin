#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

#include <kylin/lib/core/log/kylin_log.h>
#include <kylin/lib/core/log/kylin_log_buf.h>
#include <kylin/lib/core/log/kylin_log_obj.h>
#include <kylin/lib/core/log/plugin/kylin_log_console.h>
#include <kylin/lib/core/log/plugin/kylin_log_file.h>
#include <kylin/lib/core/log/plugin/kylin_log_system.h>

#define DEFAULT_LOGGER_NUM 3

klog_t *klog = NULL;

static klog_logger_t default_logger[DEFAULT_LOGGER_NUM] = {
    {
        .type = KYLIN_LOG_CONSOLE,
        .level = KYLIN_LOG_INFO,
        .print = log_console_print,
        .priv = NULL,
        .ctor = log_console_init,
        .dtor = log_console_fini
    },
    {
        .type = KYLIN_LOG_FILE,
        .level = KYLIN_LOG_INFO,
        .print = log_file_print,
        .priv = NULL,
        .ctor = log_file_init,
        .dtor = log_file_fini
    },
    {
        .type = KYLIN_LOG_SYSTEM,
        .level = KYLIN_LOG_INFO,
        .print = log_system_print,
        .priv = NULL,
        .ctor = log_system_init,
        .dtor = log_system_fini
    }
};

static uint32_t log_id;

#define KYLIN_LOG_LEVEL(level)                                 \
{                                                              \
    va_list args;                                              \
    klog_buffer_t buf;                                         \
                                                               \
    memset(&buf, 0, sizeof(klog_buffer_t));                    \
                                                               \
    va_start(args, fmt);                                       \
                                                               \
    buf.obj_id = obj->id;                                      \
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
    klog_t *obj = NULL;

    obj = kylin_malloc(sizeof(klog_t));
    if(!obj)
        return NULL;

    for(int i = 0; i < KYLIN_LOG_MAX; i++)
        obj->logger[i].type = KYLIN_LOG_MAX;

    obj->id = ++log_id;
    if(object_insert(obj))
        return NULL;

    for(int i = 0; i < num; i++) 
        kylin_log_expand(obj, &logger[i]);

    return obj;
}

void kylin_log_destroy(klog_t *obj)
{
    for(int i = 0; i < KYLIN_LOG_MAX; i++) {
        if(obj->logger[i].type == KYLIN_LOG_MAX)
            continue;

        if(obj->logger[i].dtor)
            obj->logger[i].dtor(obj->logger[i].priv);
    }

    object_remove(obj->id);
}

klog_t *kylin_log_dup(const klog_t *obj)
{
    klog_t *dup = NULL;

    dup->id = ++log_id;
    memcpy(dup->logger, obj->logger, KYLIN_LOG_MAX);
    if(object_insert(dup))
        return NULL;

    for(int i = 0; i < KYLIN_LOG_MAX; i++) {
        if(obj->logger[i].ctor)
            obj->logger[i].ctor(obj->logger[i].priv);
    }

    return dup;
}

kerr_t kylin_log_expand(klog_t *obj, klog_logger_t *logger)
{
    if(logger->type >= KYLIN_LOG_MAX ||
            logger->level > KYLIN_LOG_DEBUG ||
            logger->print == NULL)
        return KYLIN_ERROR_INVAL;

    if(obj->logger[logger->type].type == logger->type)
        return KYLIN_ERROR_EXIST;

    memcpy(&obj->logger[logger->type], logger, sizeof(klog_logger_t));

    if(logger->ctor)
        return logger->ctor(logger->priv);

    return KYLIN_ERROR_OK;
}

kerr_t kylin_log_reduce(klog_t *obj, klog_type_t type)
{
    if(obj->logger[type].type == KYLIN_LOG_MAX)
        return KYLIN_ERROR_NOENT;

    if(obj->logger[type].dtor)
        obj->logger[type].dtor(obj->logger[type].priv);

    memset(&obj->logger[type], 0, sizeof(klog_logger_t));
    obj->logger[type].type = KYLIN_LOG_MAX;

    return KYLIN_ERROR_OK;
}

klog_logger_t *kylin_log_logger_get(klog_t *obj, klog_type_t type)
{
    return &obj->logger[type];
}

klog_level_t kylin_log_level_get(klog_t *obj, klog_type_t type)
{
    return obj->logger[type].level;
}

kerr_t kylin_log_logger_set(klog_t *obj, klog_type_t type, klog_logger_t *logger)
{
    if(logger->type == KYLIN_LOG_MAX ||
            logger->level > KYLIN_LOG_DEBUG ||
            logger->print == NULL)
        return KYLIN_ERROR_INVAL;

    kylin_log_reduce(obj, type);

    return kylin_log_expand(obj, logger);
}

kerr_t kylin_log_level_set(klog_t *obj, klog_type_t type, klog_level_t level)
{
    if(obj->logger[type].type == KYLIN_LOG_MAX)
        return KYLIN_ERROR_INVAL;

    obj->logger[type].level = level;

    return KYLIN_ERROR_OK;
}

void kylin_log_debug(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_DEBUG);
}

void kylin_log_info(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_INFO);
}

void kylin_log_notice(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_NOTICE);
}

void kylin_log_warn(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_WARN);
}

void kylin_log_error(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_ERROR);
}

void kylin_log_crit(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_CRIT);
}

void kylin_log_alert(const klog_t *obj, const char *fmt, ...)
{
    KYLIN_LOG_LEVEL(KYLIN_LOG_ALERT);
}

kerr_t kylin_log_init(void) 
{
    kerr_t ret = KYLIN_ERROR_OK;

    ret = object_init();
    if(ret)
        return ret;

    ret = buffer_init();
    if(ret)
        return ret;

    klog = kylin_log_create(default_logger, DEFAULT_LOGGER_NUM);
    if(!klog)
        return KYLIN_ERROR_NOMEM;

    return ret;
}

void kylin_log_fini(void)
{
    kylin_log_destroy(klog);
    object_fini();
    buffer_fini();
}

