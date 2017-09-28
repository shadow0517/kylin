#include <kylin/include/kylin.h>

#include <kylin/lib/core/ipc/shm/kylin_shm_module.h>

#define KYLIN_SHM_MODULE_MAX             512
#define KYLIN_SHM_MODULE_OVERVIEW_FILE   "overview"

typedef struct kylin_shm_overview {
    uint32_t      count;
    kshm_module_t module[KYLIN_SHM_MODULE_MAX];
} kshm_overview_t;

kerr_t kylin_shm_module_insert(kshm_module_t *mod)
{

}

kerr_t kylin_shm_module_delete(kshm_module_t *mod)
{

}

kerr_t kylin_shm_module_delete_by_index(uint32_t index)
{

}

kerr_t kylin_shm_module_get_first(char *name)
{

}

kerr_t kylin_shm_module_get_last(char *name)
{

}

kerr_t kylin_shm_module_get_next(char *name)
{

}

kerr_t kylin_shm_module_get_prev(char *name)
{

}

kshm_module_t *kylin_shm_module_get(const char *name)
{

}

kshm_module_t *kylin_shm_module_get_by_index(uint32_t index)
{

}

