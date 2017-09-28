#ifndef _KYLIN_LOG_SYSTEM_H_
#define _KYLIN_LOG_SYSTEM_H_

#include <syslog.h>
#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

extern void log_system_print(uint64_t, klog_level_t, const char *);

extern kerr_t log_system_init(void *);
extern kerr_t log_system_fini(void *);

#endif /*_KYLIN_LOG_SYSTEM_H_*/

