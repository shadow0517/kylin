#ifndef _KYLIN_CORE_SHM_MODULE_H_
#define _KYLIN_CORE_SHM_MODULE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_limit.h>

typedef struct kylin_shm_module {
    char         name[KYLIN_NAME_LENGTH];
    size_t       cap;
    uint32_t     use;     /*引用计数*/
    kspinlock_t  lock;    /*线程安全锁*/
} kshm_module_t;

extern kerr_t kylin_shm_module_insert(kshm_module_t *);

extern kerr_t kylin_shm_module_delete(kshm_module_t *);
extern kerr_t kylin_shm_module_delete_by_index(uint32_t);

extern kerr_t kylin_shm_module_get_first(char *name);
extern kerr_t kylin_shm_module_get_last(char *name);
extern kerr_t kylin_shm_module_get_next(char *name);
extern kerr_t kylin_shm_module_get_prev(char *name);

extern kshm_module_t *kylin_shm_module_get(const char *);
extern kshm_module_t *kylin_shm_module_get_by_index(uint32_t);

#endif /*_KYLIN_CORE_SHM_MODULE_H_*/
