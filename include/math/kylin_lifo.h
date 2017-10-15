#ifndef _KYLIN_MATH_LIFO_H_
#define _KYLIN_MATH_LIFO_H_

#include <kylin/include/kylin.h>

/*
 * 后进先出队列，队列存储的是字节流
 * 自底向上生长的栈
 */

struct kylin_lifo;
typedef struct kylin_lifo klifo_t;

typedef struct {
    const char *name;
    size_t      cap;  /*容量，单位为byte*/
    struct {
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} klifo_opts_t;

#define KLIFO_OPTS_ALLOCATOR_NULL {.guard_ctor = NULL, .guard_dtor = NULL}

extern klifo_t *kylin_lifo_create(const klifo_opts_t *opts);
extern void kylin_lifo_destroy(klifo_t *);

extern int kylin_lifo_push(klifo_t *, const kbyte_t *, size_t len);
extern int kylin_lifo_pop(klifo_t *, kbyte_t *, size_t len);
extern int kylin_lifo_top(klifo_t *, kbyte_t *, size_t len);

#endif /*_KYLIN_MATH_LIFO_H_*/
