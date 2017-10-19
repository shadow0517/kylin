#ifndef _KYLIN_CORE_DAEMON_DEBUG_SERVICE_H_
#define _KYLIN_CORE_DAEMON_DEBUG_SERVICE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/include/kylin_ddb.h>

extern kddb_service_t *service_get_first(void);
extern kddb_service_t *service_get_next(kddb_service_t *);

#define service_foreach(service)                 \
    for(service = service_get_first();           \
            service != NULL;                     \
            service = service_get_next(service))

extern kerr_t service_init(void);
extern void service_fini(void);

#endif /*_KYLIN_CORE_DAEMON_DEBUG_SERVICE_H_*/
