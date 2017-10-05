#ifndef _KYLIN_CORE_LOG_CONSOLE_H_
#define _KYLIN_CORE_LOG_CONSOLE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>

extern void log_console_print(uint64_t, klog_level_t, const char *);

extern kerr_t log_console_init(void *);
extern kerr_t log_console_fini(void *);

#endif /*_KYLIN_CORE_LOG_CONSOLE_H_*/

