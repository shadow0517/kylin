#include <kylin/include/kylin.h>
#include <kylin/include/arch/x86/kylin_bitops.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/include/math/kylin_autoid.h>
#include <kylin/include/math/kylin_bitmap.h>

#define AUTOID_STATE_MAGIC 32

struct kylin_autoid {
    uint32_t   max_id;
    uint32_t   used;
    uint32_t   state[AUTOID_STATE_MAGIC]; 
    kbitmap_t *index;
};

kautoid_t *kylin_autoid_create(uint16_t max_id)
{
    kautoid_t *autoid = NULL;

    autoid = kylin_malloc(sizeof(kautoid_t));
    if(!autoid) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    if(!kylin_is_pow_of_two(max_id))
        autoid->max_id = (uint32_t)kylin_roundup_pow_of_two(max_id);

    kbitmap_opts_t autoid_opts = {
        .name = "autoid bitmap",
        .cap = autoid->max_id,
        .allocator = KBITMAP_OPTS_ALLOCATOR_NULL
    };

    autoid->index = kylin_bitmap_create(&autoid_opts);
    if(!autoid->index) {
        kylin_free(autoid);
        return NULL;
    }

    return autoid;
}

void kylin_autoid_destroy(kautoid_t *guard)
{
    if(guard->index)
        kylin_bitmap_destroy(guard->index);

    kylin_free(guard);
}

void kylin_autoid_clear(kautoid_t *guard)
{

}

int kylin_autoid_total_count(kautoid_t *guard)
{

}

int kylin_autoid_unset_count(kautoid_t *guard)
{

}

int kylin_autoid_get(kautoid_t *guard)
{

}

kerr_t kylin_autoid_put(kautoid_t *guard, uint16_t id)
{

}

kerr_t kylin_autoid_set(kautoid_t *guard, uint16_t id)
{

}

