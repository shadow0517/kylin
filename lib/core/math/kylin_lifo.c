#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_lifo.h>

struct kylin_lifo {
    klifo_opts_t opts;
    size_t       top;       /*栈顶位置*/
    kbyte_t      buf[];
};

#define LIFO_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : malloc)
#define LIFO_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : free)

klifo_t *kylin_lifo_create(const klifo_opts_t *opts)
{
    klifo_t *lifo = NULL;

    lifo = LIFO_GUARD_MALLOC(opts)(sizeof(klifo_t) + (sizeof(kbyte_t) * opts->cap));
    if(!lifo)
        return NULL;

    memcpy(&lifo->opts, opts, sizeof(klifo_opts_t));
    memset(lifo->buf, 0, sizeof(kbyte_t) * opts->cap);
    lifo->top = 0;

    return lifo;
}

void kylin_lifo_destroy(klifo_t *lifo)
{
    LIFO_GUARD_FREE(&lifo->opts)(lifo);
}

int kylin_lifo_push(klifo_t *lifo, const kbyte_t *buf, size_t len)
{
    if(lifo->top + len > (lifo->opts.cap - 1))
        return -1;

    memcpy(lifo->buf + lifo->top, buf, len);
    lifo->top += len;

    return len;
}

int kylin_lifo_pop(klifo_t *lifo, kbyte_t *buf, size_t len)
{
    if(lifo->top - len < 0)
        return -1;

    memcpy(buf, lifo->buf + lifo->top - len, len);
    lifo->top -= len;

    return len;
}

int kylin_lifo_top(klifo_t *lifo, kbyte_t *buf, size_t len)
{
    if(lifo->top - len < 0)
        return -1;

    memcpy(buf, lifo->buf + lifo->top - len, len);

    return len;
}



