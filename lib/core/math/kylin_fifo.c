#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_fifo.h>

struct kylin_fifo {
    kfifo_opts_t opts;
    size_t  cap;
    size_t  mask;
    size_t  in;
    size_t  out;
    kbyte_t val[];
};

kfifo_t *kylin_fifo_create(const kfifo_opts_t *opts)
{
    kfifo_t *fifo = NULL;

    fifo = SET_GUARD_MALLOC(opts)(sizeof(kfifo_t) + (sizeof(kmath_val_t) * opts->cap));
    if(!fifo)
        return NULL;

    memcpy(&fifo->opts, opts, sizeof(kfifo_opts_t));
    memfifo(fifo->val, 0, sizeof(kmath_val_t) * opts->cap);
    fifo->used = 0;

    return fifo;

}

void kylin_fifo_destroy(kfifo_t *fifo)
{

}

int kylin_fifo_put(kfifo_t *fifo, const kbyte_t *buf, size_t len)
{

}

int kylin_fifo_get(kfifo_t *fifo, kbyte_t *buf, size_t len)
{

}

int kylin_fifo_peek(kfifo_t *fifo, kbyte_t *buf, size_t len)
{

}

