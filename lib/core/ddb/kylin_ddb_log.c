#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/include/kylin_ddb.h>

#include <kylin/lib/core/ddb/kylin_ddb_log.h>

#define KYLIN_LOGFILE_DDB_DIR    "/var/log/ddb"
#define KYLIN_LOGFILE_DAEMON_DIR "/var/log"
#define KYLIN_LOGFILE_ARCH_MAX   9           /* archives */

#define KYLIN_LOGBUFF_MAX        2048

int kylin_ddb_log(const char *fmt, ...) 
{
    int     fd;
    int     ret = 0;
    char    logfile[PATH_MAX] = {0};
    char   *logbuf = NULL;
    va_list args;

    logbuf = alloca(KYLIN_LOGBUFF_MAX);
    if(!logbuf) 
        return -1;  

    va_start(args, fmt);

    memset(logbuf, 0, KYLIN_LOGBUFF_MAX);
    vsnprintf(logbuf, KYLIN_LOGBUFF_MAX, fmt, args);

    snprintf(logfile, PATH_MAX, "%s/%s-ddb.log", KYLIN_LOGFILE_DDB_DIR, KYLIN_PNAME);
    fd = open(logfile, O_APPEND | O_WRONLY, 00644);
    if(fd > 0) {
        write(fd, logbuf, strlen(logbuf));
        close(fd);
    } 
    else 
        ret = errno;

    va_end(args);

    return ret;
}

static kerr_t __logfile_ddb_init(void)
{
    int fd;
    char src_file[PATH_MAX]    = {0};
    char dst_file[PATH_MAX]    = {0};
    char ddb_logfile[PATH_MAX] = {0};

    snprintf(ddb_logfile, PATH_MAX, "%s/%s-ddb.log", KYLIN_LOGFILE_DDB_DIR, KYLIN_PNAME);
    fd = open(ddb_logfile, O_CREAT | O_RDONLY);
    if(fd == -1) 
        return errno;
    close(fd);

    for(int i = KYLIN_LOGFILE_ARCH_MAX; i > 0; i--) {
        snprintf(dst_file, PATH_MAX, "%s/%s-ddb.log.%d", KYLIN_LOGFILE_DDB_DIR, KYLIN_PNAME, i);
        if(i == 1)
            snprintf(src_file, PATH_MAX, "%s/%s-ddb.log", KYLIN_LOGFILE_DDB_DIR, KYLIN_PNAME);
        else 
            snprintf(src_file, PATH_MAX, "%s/%s-ddb.log.%d", KYLIN_LOGFILE_DDB_DIR, KYLIN_PNAME, i - 1);

        if(i == KYLIN_LOGFILE_ARCH_MAX) 
            unlink(dst_file);

        rename(src_file, dst_file);
    }

    return KYLIN_ERROR_OK;
}

static kerr_t __logfile_daemon_init(void)
{
    int ret __kylin_unused;
    char cmd[PATH_MAX]         = {0};
    char src_file[PATH_MAX]    = {0};
    char dst_file[PATH_MAX]    = {0};
    char ddb_logfile[PATH_MAX] = {0};

    for(int i = KYLIN_LOGFILE_ARCH_MAX; i > 0; i--) {
        snprintf(dst_file, PATH_MAX, "%s/%s.log.%d", KYLIN_LOGFILE_DAEMON_DIR, KYLIN_PNAME, i);
        if(i == 1) 
            snprintf(src_file, PATH_MAX, "%s/%s.log", KYLIN_LOGFILE_DAEMON_DIR, KYLIN_PNAME);
        else 
            snprintf(src_file, PATH_MAX, "%s/%s.log.%d", KYLIN_LOGFILE_DAEMON_DIR, KYLIN_PNAME, i - 1);

        if(i == KYLIN_LOGFILE_ARCH_MAX) 
            unlink(dst_file);

        if (i == 1) {
            snprintf(cmd, PATH_MAX, "cp %s %s &> /dev/null", src_file, dst_file);
            ret = system(cmd);
        } 
        else 
            rename(src_file, dst_file);
    }

    return KYLIN_ERROR_OK;
}

kerr_t log_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    ret = __logfile_ddb_init();
    if(ret != KYLIN_ERROR_OK)
        return ret;

    return __logfile_daemon_init();
}

void log_fini(void)
{
    return;
}

