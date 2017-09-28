#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

#include <kylin/lib/core/log/plugin/kylin_log_system.h>

static char log_buff[KYLIN_LOG_DATA_SIZE];

kerr_t log_system_init(void *arg __kylin_unused)
{
    openlog(log_buff, LOG_NDELAY, LOG_USER);
    return KYLIN_ERROR_OK;
}

kerr_t log_system_fini(void *arg __kylin_unused)
{
    closelog();
    return KYLIN_ERROR_OK;
}

void log_system_print(uint64_t timestamp, klog_level_t level, const char *buf)
{
    syslog(level | LOG_USER, buf, strlen(buf));
}

