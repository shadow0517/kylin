#ifndef _KYLIN_MATH_MAP_H_
#define _KYLIN_MATH_MAP_H_

/*
 *字典类型
 */

#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_object.h>

struct kylin_map;
struct kylin_map_node;
/*struct kylin_map_iterator;*/
typedef struct kylin_map          kmap_t;
typedef struct kylin_map_node     kmap_node_t;
/*typedef struct kylin_map_iterator kmap_iter_t;*/

typedef struct {
    const char  *name;
    void        *priv;
    kobj_t       key_type;
    size_t       key_size;

    uint64_t (*hash)(const void *key);
    int      (*key_cmp)(const void *k1, const void *k2);

    struct {
        void *(*key_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*key_dtor)(void *);
        void *(*val_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*val_dtor)(void *);
        void *(*node_ctor)(size_t);
        void  (*node_dtor)(void *);
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kmap_opts_t;

#define KMAP_OPTS_ALLOCATOR_NULL    \
{                                   \
    .key_ctor   = NULL,             \
    .key_dtor   = NULL,             \
    .val_ctor   = NULL,             \
    .val_dtor   = NULL,             \
    .node_ctor  = NULL,             \
    .node_dtor  = NULL,             \
    .guard_ctor = NULL,             \
    .guard_dtor = NULL              \
}

#define KLIST_OPTS_ALLOCATOR_KEY_VAL(kctor, vctor)  \
{                                                   \
    .key_ctor   = kctor,                            \
    .key_dtor   = NULL,                             \
    .val_ctor   = vctor,                            \
    .val_dtor   = NULL,                             \
    .node_ctor  = NULL,                             \
    .node_dtor  = NULL,                             \
    .guard_ctor = NULL,                             \
    .guard_dtor = NULL                              \
}


extern kmap_t *kylin_map_create(const kmap_opts_t *opts) 
    __kylin_nonnull((1));
extern void kylin_map_destroy(kmap_t *) 
    __kylin_nonnull((1));
extern void kylin_map_clear(kmap_t *) 
    __kylin_nonnull((1));

extern void *kylin_map_priv(kmap_t *) 
    __kylin_nonnull((1));

extern void *kylin_map_key(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));
extern void *kylin_map_val(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));
extern kobj_t kylin_map_val_type(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));

extern size_t kylin_map_count(kmap_t *) 
    __kylin_nonnull((1));
extern kerr_t kylin_map_resize(kmap_t *, size_t) 
    __kylin_nonnull((1));

extern kmap_node_t *kylin_map_insert(kmap_t *, void *key, 
        void *val, kobj_t val_type, size_t size) 
    __kylin_nonnull((1, 2, 3));
extern kmap_node_t *kylin_map_insert_raw(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));

extern kerr_t kylin_map_remove(kmap_t *, void *) 
    __kylin_nonnull((1, 2));
extern kerr_t kylin_map_remove_raw(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));

extern void *kylin_map_unlink(kmap_t *, void *key) 
    __kylin_nonnull((1, 2));
extern kmap_node_t *kylin_map_unlink_raw(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));

extern kmap_node_t *kylin_map_replace(kmap_t *, void *key, 
        void *val, kobj_t val_type, size_t size) 
    __kylin_nonnull((1, 2, 3));

extern kmap_node_t *kylin_map_find(kmap_t *, void *key) 
    __kylin_nonnull((1, 2));
extern kmap_node_t *kylin_map_first(kmap_t *) 
    __kylin_nonnull((1));
extern kmap_node_t *kylin_map_last(kmap_t *) 
    __kylin_nonnull((1));
extern kmap_node_t *kylin_map_next(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));
extern kmap_node_t *kylin_map_prev(kmap_t *, kmap_node_t *) 
    __kylin_nonnull((1, 2));

extern void *kylin_map_fetch_val(kmap_t *, void *key) 
    __kylin_nonnull((1, 2));

#endif /*_KYLIN_MATH_MAP_H_*/
