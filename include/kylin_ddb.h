#ifndef _KYLIN_DAEMON_DEBUG_H_
#define _KYLIN_DAEMON_DEBUG_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

extern int kylin_ddb_log(const char *fmt, ...) __kylin_format_print(1, 2);

typedef struct {
    void (exec)(void);
} kddb_service_t;

extern kerr_t kylin_ddb_service_register(kddb_service_t *);
extern void kylin_ddb_service_unregister(kddb_service_t *);

#endif /*_KYLIN_DAEMON_DEBUG_H_*/
