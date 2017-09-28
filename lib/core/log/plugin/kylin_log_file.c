#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

#include <kylin/lib/core/log/plugin/kylin_log_file.h>

static char log_buff[KYLIN_LOG_DATA_SIZE];
static int log_fd = -1;

kerr_t log_file_init(void *arg)
{
    log_fd = open((char *)arg, O_CREAT | O_APPEND | O_WRONLY, 00644);
    if(log_fd == -1) {
        return KYLIN_ERROR_NOENT;
    }

    memset(log_buff, 0, KYLIN_LOG_DATA_SIZE);
    return KYLIN_ERROR_OK;
}

kerr_t log_file_fini(void *arg __kylin_unused);
{
    if(log_fd > 0) {
        close(log_fd);
    }
    return KYLIN_ERROR_OK;
}

void log_file_print(uint64_t timestamp, klog_level_t level, const char *buf)
{
    snprintf(log_buff, KYLIN_LOG_DATA_SIZE - 1, "%s\t%s\t: %s\n", 
            timestamp, kylin_log_level_string(level),buf);
    if(log_fd > 0) {
        write(log_fd, log_buff, strlen(log_buff));
    }
}
