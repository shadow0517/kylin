#ifndef _KYLIN_MATH_INDEX_H_
#define _KYLIN_MATH_INDEX_H_

#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_object.h>

/*
 * 哈希索引
 */

struct kylin_index;
struct kylin_index_node;
typedef struct kylin_index      kindex_t;
typedef struct kylin_index_node kindex_node_t;

typedef struct {
    const char  *name;
    void        *priv;
    kobj_t       val_type;
    size_t       val_size;

    uint64_t (*hash)(const void *val);
    int      (*compare)(const void *v1, const void *v2);

    struct {
        void *(*val_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*val_dtor)(void *);
        void *(*node_ctor)(size_t);
        void  (*node_dtor)(void *);
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kindex_opts_t;

#define KINDEX_OPTS_ALLOCATOR_NULL  \
{                                   \
    .val_ctor   = NULL,             \
    .val_dtor   = NULL,             \
    .node_ctor  = NULL,             \
    .node_dtor  = NULL,             \
    .guard_ctor = NULL,             \
    .guard_dtor = NULL              \
}

#define KINDEX_OPTS_ALLOCATOR_VAL(ctor) \
{                                       \
    .val_ctor   = ctor,                 \
    .val_dtor   = NULL,                 \
    .node_ctor  = NULL,                 \
    .node_dtor  = NULL,                 \
    .guard_ctor = NULL,                 \
    .guard_dtor = NULL                  \
}

extern kindex_t *kylin_index_create(const kindex_opts_t *opts) 
    __kylin_nonnull((1));
extern void kylin_index_destroy(kindex_t *) 
    __kylin_nonnull((1));

extern void *kylin_index_priv(kindex_t *) 
    __kylin_nonnull((1));

extern kobj_t kylin_index_val_type(kindex_t *) 
    __kylin_nonnull((1));
extern size_t kylin_index_val_size(kindex_t *) 
    __kylin_nonnull((1));

extern void *kylin_index_val(kindex_t *, kindex_node_t *) 
    __kylin_nonnull((1, 2));

extern size_t kylin_index_count(kindex_t *) 
    __kylin_nonnull((1));
extern kerr_t kylin_index_resize(kindex_t *, size_t) 
    __kylin_nonnull((1));

extern void kylin_index_clear(kindex_t *) 
    __kylin_nonnull((1));

extern kindex_node_t *kylin_index_insert(kindex_t *, void *val) 
    __kylin_nonnull((1, 2));
extern kindex_node_t *kylin_index_insert_raw(kindex_t *, kindex_node_t *) 
    __kylin_nonnull((1, 2));

extern kerr_t kylin_index_remove(kindex_t *, void *cmp) 
    __kylin_nonnull((1, 2));
extern kerr_t kylin_index_remove_raw(kindex_t *, kindex_node_t *) 
    __kylin_nonnull((1, 2));

extern void *kylin_index_unlink(kindex_t *, void *) 
    __kylin_nonnull((1, 2));
extern kindex_node_t *kylin_index_unlink_raw(kindex_t *, kindex_node_t *) 
    __kylin_nonnull((1, 2));

extern kindex_node_t *kylin_index_replace(kindex_t *, void *val) 
    __kylin_nonnull((1, 2));

extern kindex_node_t *kylin_index_find(kindex_t *, void *key) 
    __kylin_nonnull((1, 2));
extern kindex_node_t *kylin_index_first(kindex_t *) 
    __kylin_nonnull((1));
extern kindex_node_t *kylin_index_last(kindex_t *) 
    __kylin_nonnull((1));
extern kindex_node_t *kylin_index_next(kindex_t *, kindex_node_t *) 
    __kylin_nonnull((1, 2));
extern kindex_node_t *kylin_index_prev(kindex_t *, kindex_node_t *) 
    __kylin_nonnull((1, 2));

#endif /*_KYLIN_MATH_INDEX_H_*/
