#ifndef _KYLIN_CORE_MODULE_H_
#define _KYLIN_CORE_MODULE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t module_init(void);
extern void module_fini(void);

#endif /*_KYLIN_CORE_MODULE_H_*/
