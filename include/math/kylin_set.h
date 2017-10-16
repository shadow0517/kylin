#ifndef _KYLIN_MATH_SET_H_
#define _KYLIN_MATH_SET_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_object.h>

/*
 * 集合
 */
struct kylin_set;
/*struct kylin_set_iterator;*/
typedef struct kylin_set          kset_t;
/*typedef struct kylin_set_iterator kset_iter_t;*/

typedef struct {
    const char *name;
    kbool_t     is_order;            /*是否为有序集合(升序排列)，KYLIN_TRUE or KYLIN_FALSE*/
    size_t      cap;                 /*容量*/
    kobj_t      val_type;
    size_t      val_size; 

    int (*compare)(const void *v1, const void *v2);

    struct {
        void *(*val_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*val_dtor)(void *);
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kset_opts_t;

#define KSET_OPTS_ALLOCATOR_NULL    \
{                                   \
    .val_ctor   = NULL,             \
    .val_dtor   = NULL,             \
    .guard_ctor = NULL,             \
    .guard_dtor = NULL              \
}

#define KSET_OPTS_ALLOCATOR_VAL(ctor)   \
{                                       \
    .val_ctor   = ctor,                 \
    .val_dtor   = NULL,                 \
    .guard_ctor = NULL,                 \
    .guard_dtor = NULL                  \
}

extern kset_t *kylin_set_create(const kset_opts_t *opts) 
    __kylin_nonnull((1));
extern void kylin_set_destroy(kset_t *) 
    __kylin_nonnull((1));
extern void kylin_set_clear(kset_t *) 
    __kylin_nonnull((1));

extern kobj_t kylin_set_val_type(kset_t *) 
    __kylin_nonnull((1));
extern size_t kylin_set_val_size(kset_t *) 
    __kylin_nonnull((1));
extern size_t kylin_set_count(kset_t *) 
    __kylin_nonnull((1));

extern kerr_t kylin_set_insert(kset_t *, void *) 
    __kylin_nonnull((1, 2));

extern kerr_t kylin_set_remove(kset_t *, void *) 
    __kylin_nonnull((1, 2));
extern kerr_t kylin_set_remove_by_index(kset_t *, uint32_t) 
    __kylin_nonnull((1));

extern void *kylin_set_unlink(kset_t *, void *) 
    __kylin_nonnull((1, 2));
extern void *kylin_set_unlink_by_index(kset_t *, uint32_t) 
    __kylin_nonnull((1));

extern void *kylin_set_find(kset_t *, void *) 
    __kylin_nonnull((1, 2));
extern void *kylin_set_find_by_index(kset_t *, uint32_t) 
    __kylin_nonnull((1));

extern void *kylin_set_random(kset_t *) 
    __kylin_nonnull((1));
extern void *kylin_set_first(kset_t *) 
    __kylin_nonnull((1));
extern void *kylin_set_last(kset_t *) 
    __kylin_nonnull((1));
extern void *kylin_set_next(kset_t *, void *) 
    __kylin_nonnull((1, 2));
extern void *kylin_set_prev(kset_t *, void *) 
    __kylin_nonnull((1, 2));

#define KYLIN_SET_FOREACH(guard, val)          \
    for(val = kylin_set_first(guard);          \
            val != NULL;                       \
            val = kylin_set_next(guard, val))

/*排序, 对于非顺序的集合，该操作可能花费一定的时间*/
extern kerr_t kylin_set_order(kset_t *) 
    __kylin_nonnull((1));

#endif /*_KYLIN_MATH_SET_H_*/
