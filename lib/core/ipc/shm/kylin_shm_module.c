#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_limit.h>

#include <kylin/lib/core/ipc/shm/kylin_shm_module.h>

#define KYLIN_SHM_MODULE_MAX  512
#define KYLIN_SHM_MODULE_OVERVIEW_FILE   "overview"

typedef struct kylin_shm_module {
    char         name[KYLIN_NAME_LENGTH];
    size_t       cap;
    uint32_t     use;     /*引用计数*/
    kspinlock_t  lock;    /*线程安全锁*/
} kshm_module_t;

typedef struct kylin_shm_overview {
    uint32_t      count;
    kshm_module_t module[KYLIN_SHM_MODULE_MAX];
} kshm_overview_t;


