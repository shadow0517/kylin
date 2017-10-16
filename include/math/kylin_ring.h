#ifndef _KYLIN_MATH_BUFRING_H_
#define _KYLIN_MATH_BUFRING_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_object.h>

/*
 * 环形缓冲区
 */

struct kylin_ring;
typedef struct kylin_ring kring_t;

typedef struct {
    const char *name;
    size_t      cap; /*容量，对象的个数，大小必须为2的幂次方*/
    kobj_t      val_type;
    size_t      val_size;

    struct {
        void *(*val_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*val_dtor)(void *);
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kring_opts_t;

#define KRING_OPTS_ALLOCATOR_NULL   \
{                                   \
    .val_ctor   = NULL,             \
    .val_dtor   = NULL,             \
    .guard_ctor = NULL,             \
    .guard_dtor = NULL              \
}

#define KRING_OPTS_ALLOCATOR_VAL(ctor)  \
{                                       \
    .val_ctor   = ctor,                 \
    .val_dtor   = NULL,                 \
    .guard_ctor = NULL,                 \
    .guard_dtor = NULL                  \
}

extern kring_t *kylin_ring_create(kring_opts_t *opts) 
    __kylin_nonnull((1));
extern void kylin_ring_destroy(kring_t *) 
    __kylin_nonnull((1));

extern kerr_t kylin_ring_enqueue(kring_t *, void *) 
    __kylin_nonnull((1, 2));
extern void *kylin_ring_dequeue(kring_t *) 
    __kylin_nonnull((1));
extern void *kylin_ring_peek(kring_t *) 
    __kylin_nonnull((1));

#endif /*_KYLIN_MATH_BUFRING_H_*/
