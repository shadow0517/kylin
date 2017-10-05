#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

#include <kylin/lib/core/log/plugin/kylin_log_console.h>

kerr_t log_console_init(void *arg __kylin_unused)
{
    return KYLIN_ERROR_OK;
}

kerr_t log_console_fini(void *arg __kylin_unused)
{
    return KYLIN_ERROR_OK;
}

void log_console_print(uint64_t timestamp, klog_level_t level, const char *buf)
{
    printf("%lu %d: %s\n", timestamp, level, buf);
}


