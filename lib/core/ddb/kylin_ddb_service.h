#ifndef _KYLIN_CORE_DAEMON_DEBUG_SERVICE_H_
#define _KYLIN_CORE_DAEMON_DEBUG_SERVICE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t kylin_ddb_service_init(void);
extern void kylin_ddb_service_fini(void);

#endif /*_KYLIN_CORE_DAEMON_DEBUG_SERVICE_H_*/
