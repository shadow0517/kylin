#ifndef _KYLIN_MATH_FIFO_H_
#define _KYLIN_MATH_FIFO_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

/*
 * 先进先出队列，队列存储的是字节流
 */
struct kylin_fifo;
typedef struct kylin_fifo kfifo_t;

typedef struct {
    const char *name;
    size_t      cap; /*容量，单位为byte，大小必须为2的幂次方*/
    struct {
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kfifo_opts_t;

#define KFIFO_OPTS_ALLOCATOR_NULL {.guard_ctor = NULL, .guard_dtor = NULL}

extern kfifo_t *kylin_fifo_create(const kfifo_opts_t *opts) 
    __kylin_nonnull((1));
extern void kylin_fifo_destroy(kfifo_t *) 
    __kylin_nonnull((1));

extern int kylin_fifo_put(kfifo_t *, const kbyte_t *, size_t len) 
    __kylin_nonnull((1, 2));
extern int kylin_fifo_get(kfifo_t *, kbyte_t *, size_t len) 
    __kylin_nonnull((1, 2));
extern int kylin_fifo_peek(kfifo_t *, kbyte_t *, size_t len) 
    __kylin_nonnull((1, 2));

#endif /*_KYLIN_MATH_FIFO_H_*/
