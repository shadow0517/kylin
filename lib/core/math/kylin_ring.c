#include <kylin/include/kylin.h>
#include <kylin/include/arch/x86/kylin_atomic.h>
#include <kylin/include/math/kylin_ring.h>

#include <kylin/lib/core/math/kylin_math.h>

struct kylin_ring {
    kring_opts_t  opts;
    size_t        mask;

    katomic32_t   head;
    katomic32_t   tail;

    kmath_val_t   val[];
};

#define RING_VAL_MALLOC(opts)    ((opts)->allocator.val_ctor)
#define RING_VAL_FREE(opts)      ((opts)->allocator.val_dtor   ? (opts)->allocator.val_dtor   : free)
#define RING_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : malloc)
#define RING_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : free)

kring_t *kylin_ring_create(kring_opts_t *opts)
{
    kring_t *ring = NULL;

    ring = RING_GUARD_MALLOC(opts)(sizeof(kring_t) + (sizeof(kmath_val_t) * opts->cap));
    if(!ring)
        return NULL;

    memcpy(&ring->opts, opts, sizeof(kring_opts_t));
    ring->mask = opts->cap - 1;
    kylin_atomic32_init(&ring->head);
    kylin_atomic32_init(&ring->tail);
    memset(ring->val, 0, sizeof(kmath_val_t) * opts->cap);

    return ring;
}

void kylin_ring_destroy(kring_t *guard)
{
    uint32_t head;
    uint32_t tail;
    uint32_t new_tail;

    while(1) {
        tail = kylin_atomic32_read(&guard->tail);
        head = kylin_atomic32_read(&guard->head);
        if(tail == head)
            break;

        new_tail = (tail == guard->mask ? 0 : tail + 1);
        if(kylin_atomic32_cmpset(&guard->tail.cnt, tail, new_tail)) {
            RING_VAL_FREE(&guard->opts)(kmath_val_get(&guard->val[new_tail], guard->opts.val_type));   
        }
    }

    RING_GUARD_FREE(&guard->opts)(guard);
}

kerr_t kylin_ring_enqueue(kring_t *guard, void *buf)
{
    kerr_t ret = KYLIN_ERROR_NOMEM;
    uint32_t head;
    uint32_t tail;
    uint32_t new_head;

    while(1) {
        head = kylin_atomic32_read(&guard->head);
        tail = kylin_atomic32_read(&guard->tail);

        new_head = (head == guard->mask ? 0 : head + 1);
        if(new_head == tail)
            break;

        if(kylin_atomic32_cmpset(&guard->head.cnt, head, new_head)) {
            kmath_val_ctor(&guard->val[new_head], buf, 
                    guard->opts.val_type, guard->opts.val_size, RING_VAL_MALLOC(&guard->opts));  
            ret = KYLIN_ERROR_OK;
            break;
        }
    }

    return ret;
}

void *kylin_ring_dequeue(kring_t *guard)
{
    uint32_t head;
    uint32_t tail;
    uint32_t new_tail;
    void    *buf = NULL;

    while(1) {
        tail = kylin_atomic32_read(&guard->tail);
        head = kylin_atomic32_read(&guard->head);
        if(tail == head)
            break;

        new_tail = (tail == guard->mask ? 0 : tail + 1);
        if(kylin_atomic32_cmpset(&guard->tail.cnt, tail, new_tail)) {
            buf = kmath_val_get(&guard->val[new_tail], guard->opts.val_type);
            break;
        }
    }

    return buf;
}

void *kylin_ring_peek(kring_t *guard)
{
    uint32_t head;
    uint32_t tail;
    uint32_t new_tail;
    void    *buf = NULL;

    while(1) {
        tail = kylin_atomic32_read(&guard->tail);
        head = kylin_atomic32_read(&guard->head);
        if(tail == head)
            break;

        new_tail = (tail == guard->mask ? 0 : tail + 1);
        buf = kmath_val_get(&guard->val[new_tail], guard->opts.val_type);
        break;
    }

    return buf;
}


