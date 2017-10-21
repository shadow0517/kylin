#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_fifo.h>

struct kylin_fifo {
    kfifo_opts_t opts;
    size_t       mask;
    size_t       in;
    size_t       out;
    kbool_t      is_full; /*处理in = out的情况*/
    kbyte_t      val[];
};

#define FIFO_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : kylin_malloc)
#define FIFO_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : kylin_free)

kfifo_t *kylin_fifo_create(const kfifo_opts_t *opts)
{
    kfifo_t *fifo = NULL;

    if(!kylin_is_pow_of_two(opts->cap))
        return NULL;

    fifo = FIFO_GUARD_MALLOC(opts)(sizeof(kfifo_t) + (sizeof(kbyte_t) * opts->cap));
    if(!fifo)
        return NULL;

    memcpy(&fifo->opts, opts, sizeof(kfifo_opts_t));
    memset(fifo->val, 0, sizeof(kbyte_t) * opts->cap);
    fifo->mask    = opts->cap - 1;
    fifo->in      = 0;
    fifo->out     = 0;
    fifo->is_full = KYLIN_FALSE;

    return fifo;
}

void kylin_fifo_destroy(kfifo_t *fifo)
{
    FIFO_GUARD_FREE(&fifo->opts)(fifo);
}

int kylin_fifo_put(kfifo_t *fifo, const kbyte_t *buf, size_t len)
{
    if((len == 0) ||
            (fifo->is_full) ||
            (fifo->in < fifo->out && fifo->in + len > fifo->out) ||
            (fifo->in >= fifo->out && fifo->in + len > fifo->mask && ((fifo->in + len) & fifo->mask) > fifo->out))
        return -1;

    if(fifo->in + len > fifo->opts.cap) {
        size_t rest = fifo->opts.cap - fifo->in;

        memcpy(fifo->val + fifo->in, buf, rest);
        memcpy(fifo->val, buf + rest, len - rest);

        fifo->in = len - rest;
        if(fifo->in == fifo->out)
            fifo->is_full = KYLIN_TRUE;
    }
    else {
        memcpy(fifo->val + fifo->in, buf, len);
        fifo->in = (fifo->in + len) & fifo->mask;
    }

    return len;
}

int kylin_fifo_get(kfifo_t *fifo, kbyte_t *buf, size_t len)
{
    if((len == 0) ||
            (len > fifo->opts.cap) ||
            (!fifo->is_full && len > (fifo->in >= fifo->out ? 
                                      fifo->in - fifo->out : 
                                      fifo->opts.cap - (fifo->out - fifo->in)))) 
        return -1;

    if(fifo->out + len > fifo->opts.cap) {
        size_t rest = fifo->opts.cap - fifo->out;

        memcpy(buf, fifo->val + fifo->out, rest);
        memcpy(buf + rest, fifo->val, len - rest);

        fifo->out = len - rest;
    }
    else {
        memcpy(buf, fifo->val + fifo->out, len);
        fifo->out = (fifo->out + len) & fifo->mask;
    }

    if(fifo->is_full)
        fifo->is_full = KYLIN_FALSE;

    return len;
}

int kylin_fifo_peek(kfifo_t *fifo, kbyte_t *buf, size_t len)
{
    if((len == 0) ||
            (len > fifo->opts.cap) ||
            (!fifo->is_full && len > (fifo->in >= fifo->out ? 
                                      fifo->in - fifo->out : 
                                      fifo->opts.cap - (fifo->out - fifo->in)))) 
        return -1;

    if(fifo->out + len > fifo->opts.cap) {
        size_t rest = fifo->opts.cap - fifo->out;

        memcpy(buf, fifo->val + fifo->out, rest);
        memcpy(buf + rest, fifo->val, len - rest);

    }
    else {
        memcpy(buf, fifo->val + fifo->out, len);
    }

    return len;
}

