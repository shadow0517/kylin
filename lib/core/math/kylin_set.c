#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_set.h>

#include <kylin/lib/core/math/kylin_math.h>

struct kylin_set {
    kset_opts_t  opts;
    size_t       used;
    kmath_val_t  val[];
};

#define SET_VAL_MALLOC(opts)    ((opts)->allocator.val_ctor)
#define SET_VAL_FREE(opts)      ((opts)->allocator.val_dtor   ? (opts)->allocator.val_dtor   : free)
#define SET_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : malloc)
#define SET_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : free)

kset_t *kylin_set_create(const kset_opts_t *opts)
{
    kset_t *set = NULL;

    set = SET_GUARD_MALLOC(opts)(sizeof(kset_t) + (sizeof(kmath_val_t) * opts->cap));
    if(!set)
        return NULL;

    memcpy(&set->opts, opts, sizeof(kset_opts_t));
    memset(set->val, 0, sizeof(kmath_val_t) * opts->cap);
    set->used = 0;

    return set;
}

void kylin_set_destroy(kset_t *guard)
{
    kylin_set_clear(guard);
    SET_GUARD_FREE(&guard->opts)(guard);
}

void kylin_set_clear(kset_t *guard)
{
    for(int i = 0; i < guard->used; i++)
        kmath_val_dtor(&guard->val[i], guard->opts.val_type, SET_VAL_FREE(&guard->opts));
}

kobj_t kylin_set_val_type(kset_t *guard)
{
    return guard->opts.val_type;
}

size_t kylin_set_val_size(kset_t *guard)
{
    return guard->opts.val_size;
}

size_t kylin_set_count(kset_t *guard)
{
    return guard->used;
}

kerr_t kylin_set_insert(kset_t *guard, void *v)
{
    kmath_val_t val;

    if(guard->used == guard->opts.cap)
        return KYLIN_ERROR_INVAL;

    kmath_val_ctor(&val, v, guard->opts.val_type, guard->opts.val_size, SET_VAL_MALLOC(&guard->opts));

    if(guard->opts.is_order) {
        for(int i = 0; i < guard->used; i++) {
            if(guard->opts.compare && 
                    guard->opts.compare(&val, kmath_val_get(&guard->val[i], guard->opts.val_type)) < 0) {
                for(int j = i; j < guard->used; j++)
                    KYLIN_SWAP(val, guard->val[j]);
                break;
            }
        }
    }
    guard->val[guard->used++] = val;

    return KYLIN_ERROR_OK;
}

kerr_t kylin_set_remove(kset_t *guard, void *cmp)
{
    for(int i = 0; i < guard->used; i++) {
        if(guard->opts.compare && 
                guard->opts.compare(cmp, kmath_val_get(&guard->val[i], guard->opts.val_type)) == 0)
            return kylin_set_remove_by_index(guard, i);
    }
    return KYLIN_ERROR_NOENT; 
}

kerr_t kylin_set_remove_by_index(kset_t *guard, uint32_t index)
{
    if(index >= guard->used)
        return KYLIN_ERROR_INVAL;

    for(int i = index; i < (guard->used - 1); i++)
        KYLIN_SWAP(guard->val[i], guard->val[i + 1]);

    guard->used--;

    return KYLIN_ERROR_OK;
}

void *kylin_set_find(kset_t *guard, void *key)
{
    for(int i = 0; i < guard->used; i++) {
        if(guard->opts.compare && 
                guard->opts.compare(key, kmath_val_get(&guard->val[i], guard->opts.val_type)) == 0)
            return result;
    }

    return NULL;
}

void *kylin_set_find_by_index(kset_t *guard, uint32_t index)
{
    return kmath_val_get(&guard->val[index], guard->opts.val_type);
}

void *kylin_set_random(kset_t *guard)
{
    return kmath_val_get(&guard->val[rand() % guard->used], guard->opts.val_type);
}

void *kylin_set_first(kset_t *guard)
{
    return kmath_val_get(&guard->val[0], guard->opts.val_type);
}

void *kylin_set_last(kset_t *guard)
{
    return kmath_val_get(&guard->val[guard->used - 1], guard->opts.val_type);
}

void *kylin_set_next(kset_t *guard, void *cmp)
{
    for(int i = 0; i < guard->used; i++) {
        if(guard->opts.compare && 
                guard->opts.compare(cmp, kmath_val_get(&guard->val[i], guard->opts.val_type)) == 0) {
            if(i < (guard->used - 1))
                return kmath_val_get(&guard->val[i + 1], guard->opts.val_type);
            else
                return NULL;
        }
    }

    return NULL;
}

void *kylin_set_prev(kset_t *guard, void *cmp)
{
    for(int i = 0; i < guard->used; i++) {
        if(guard->opts.compare && 
                guard->opts.compare(cmp, kmath_val_get(&guard->val[i], guard->opts.val_type)) == 0) {
            if(i > 0)
                return kmath_val_get(&guard->val[i - 1], guard->opts.val_type);
            else
                return NULL;
        }
    }

    return NULL;
}

/*
 * 冒泡排序
 */
static __kylin_inline kerr_t _bubble_sort(kmath_val_t *val, size_t arr_len)
{
    return KYLIN_ERROR_OK;
}

/*
 * 快速排序
 */
static __kylin_inline kerr_t _quick_sort(kmath_val_t *val, size_t arr_len)
{
    return KYLIN_ERROR_OK;
}

/*
 * 以时间复杂度计算，当集合元素的数量少于N时，采用冒泡排序，否则采用快速排序
 */
kerr_t kylin_set_order(kset_t *guard)
{
    kerr_t ret = KYLIN_ERROR_OK;

    if(guard->used < 100)
        ret = _bubble_sort(guard->val, guard->used);
    else
        ret = _quick_sort(guard->val, guard->used);

    return ret;
}

