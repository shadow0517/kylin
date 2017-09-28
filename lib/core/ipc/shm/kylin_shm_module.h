#ifndef _KYLIN_CORE_SHM_MODULE_H_
#define _KYLIN_CORE_SHM_MODULE_H_

struct kylin_shm_module;
typedef struct kylin_shm_module kshm_module_t;

extern kshm_module_t *kylin_shm_module_get_by_index(uint32_t);

#endif /*_KYLIN_CORE_SHM_MODULE_H_*/
