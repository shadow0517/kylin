#ifndef _KYLIN_CORE_SHM_OBJ_H_
#define _KYLIN_CORE_SHM_OBJ_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

typedef struct {
    char         name[KYLIN_NAME_LENGTH];
    size_t       cap;
    uint32_t     refcnt;  /*引用计数*/
} kshm_obj_t;

extern kerr_t object_insert(kshm_obj_t *);

extern kerr_t object_delete(kshm_obj_t *);
extern kerr_t object_delete_by_index(uint32_t);

extern kshm_obj_t *object_find(const char *);
extern kshm_obj_t *object_find_by_index(uint32_t);

extern kerr_t object_init(void);
extern void object_fini(void);

#endif /*_KYLIN_CORE_SHM_OBJ_H_*/
