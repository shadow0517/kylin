#ifndef _KYLIN_DAEMON_DEBUG_H_
#define _KYLIN_DAEMON_DEBUG_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

typedef enum {
    KDDB_STATUS_NONE     = 0x00,
    KDDB_STATUS_RUNNING,
    KDDB_STATUS_STOPPED,
    KDDB_STATUS_CORE,
    KDDB_STATUS_UNKNOWN,
    KDDB_STATUS_MAX
} kddb_status_t;

typedef struct {
    char     name[KYLIN_NAME_LENGTH];
    uint16_t runs;

#define KYLIN_DDB_PROC_CURRENT 0
#define KYLIN_DDB_PROC_BEFORE  1
    struct {
        kddb_status_t status;
        uint8_t       signal;
        uint64_t      start;
        uint64_t      stop;
    } proc[2];
} kddb_t;

extern kddb_t *kylin_ddb_get(void);

extern int kylin_ddb_log(const char *fmt, ...) __kylin_format_print(1, 2);

typedef struct {
    void (*exec)(void);
} kddb_service_t;

extern kerr_t kylin_ddb_service_register(kddb_service_t *);
extern void kylin_ddb_service_unregister(kddb_service_t *);

#endif /*_KYLIN_DAEMON_DEBUG_H_*/
