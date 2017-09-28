#ifndef _KYLIN_LOG_FILE_H_
#define _KYLIN_LOG_FILE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

extern void log_file_print(uint64_t, klog_level_t, const char *);

extern kerr_t log_file_init(void *);
extern kerr_t log_file_fini(void *);

#endif /*_KYLIN_LOG_FILE_H_*/
