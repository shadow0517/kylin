#include <kylin/include/kylin.h>
#include <kylin/include/arch/kylin_atomic.h>
#include <kylin/include/math/kylin_ring.h>

struct kylin_ring {
    kring_opts_t  opts;
    size_t        mask;

    katomic32_t   prod_head;
    katomic32_t   prod_tail;
    katomic32_t   cons_head;
    katomic32_t   cons_tail;

    kmath_val_t   ring[];
};

kring_t *kylin_ring_create(kring_opts_t *opts, size_t count)
{

}

void kylin_ring_destroy(kring_t *)
{

}

kerr_t kylin_ring_enqueue(kring_t *, void *)
{

}

void *kylin_ring_dequeue(kring_t *)
{

}

void *kylin_ring_peek(kring_t *)
{

}


