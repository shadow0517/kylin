#ifndef _KYLIN_CORE_DAEMON_DEBUG_H_
#define _KYLIN_CORE_DAEMON_DEBUG_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_ddb.h>

typedef enum {
    KDDB_STATUS_NONE     = 0x00,
    KDDB_STATUS_RUNNING,
    KDDB_STATUS_STOPPED,
    KDDB_STATUS_RUNNING,
    KDDB_STATUS_CORE,
    KDDB_STATUS_UNKNOWN,
    KDDB_STATUS_MAX
} kddb_status_t;

extern void kylin_ddb_set_status(kddb_status_t);
extern kddb_status_t kylin_ddb_get_status(void);

extern kerr_t kylin_ddb_init(void);
extern void kylin_ddb_fini(void);

#endif /*_KYLIN_CORE_DAEMON_DEBUG_H_*/
