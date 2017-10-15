#ifndef _KYLIN_MATH_NORMAL_H_
#define _KYLIN_MATH_NORMAL_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_object.h>

#include <kylin/include/math/kylin_index.h>
#include <kylin/include/math/kylin_map.h>
#include <kylin/include/math/kylin_set.h>
#include <kylin/include/math/kylin_string.h>

/*
 * 对象值的通用表现形式
 */
typedef union {
    void     *v;
    uint64_t  u64;
    int64_t   s64;
    double    d;
} kmath_val_t;

static __kylin_inline void kmath_val_ctor(kmath_val_t *val, void *v, 
        kobj_t type, size_t size, void *(*ctor)(size_t))
{
    switch(type) {
        case KOBJ_INT8:
            val->s64 = *((int8_t *)v);
            break;
        case KOBJ_INT16:
            val->s64 = *((int16_t *)v);
            break;
        case KOBJ_INT32:
            val->s64 = *((int32_t *)v);
            break;
        case KOBJ_INT64:
            val->s64 = *((int64_t *)v);
            break;
        case KOBJ_UINT8:
            val->u64 = *((uint8_t *)v);
            break;
        case KOBJ_UINT16:
            val->u64 = *((uint16_t *)v);
            break;
        case KOBJ_UINT32:
            val->u64 = *((uint32_t *)v);
            break;
        case KOBJ_UINT64:
            val->u64 = *((uint64_t *)v);
            break;
        case KOBJ_STRING:
        case KOBJ_KMAP:
        case KOBJ_KSET:
        case KOBJ_KSTR:
        case KOBJ_OTHERS:
            if(ctor){
                val->v = ctor(size);
                memcpy(val->v, v, size);
            }
            else
                val->v = v;
            break;
        default:
            val->v = v;
    }
    return ;
}

static __kylin_inline void kmath_val_dtor(kmath_val_t *val, kobj_t type, void (*dtor)(void *))
{
    switch(type) {
        case KOBJ_INT8:
        case KOBJ_INT16:
        case KOBJ_INT32:
        case KOBJ_INT64:
        case KOBJ_UINT8:
        case KOBJ_UINT16:
        case KOBJ_UINT32:
        case KOBJ_UINT64:
            break;
        case KOBJ_KMAP:
            kylin_map_destroy((kmap_t *)val->v);
            break;
        case KOBJ_KSTR:
            kylin_string_destroy((kstr_t *)val->v);
            break;
        case KOBJ_KSET:
            kylin_set_destroy((kset_t *)val->v);
            break;
        default:
            if(dtor)
                dtor(val->v);
            break;
    }
    return ;
}

static __kylin_inline int kmath_val_cmp(const kmath_val_t *v1, const kmath_val_t *v2, kobj_t type, 
        int (*cmp)(const void *, const void *)) 
{
    int result = 0;

    if(type < KOBJ_NORMAL) {
        if(type < KOBJ_SIGNED)
            result = v1->s64 > v2->s64 ? 1: -1;
        else if(type < KOBJ_UNSIGNED)
            result = v1->u64 > v2->u64 ? 1: -1;
        else
            result = v1->d > v2->d ? 1: -1;
    }
    else
        result = cmp(v1->v, v2->v);

    return result;
}

static __kylin_inline int kmath_val_cmp_raw(const kmath_val_t *v1, const void *v2, kobj_t type,
        int (*cmp)(const void *, const void *))
{
    int result = 0;

    if(type < KOBJ_NORMAL) {
        if(type < KOBJ_SIGNED)
            result = v1->s64 > *((const int64_t *)v2) ? 1 : -1; 
        else if(type < KOBJ_UNSIGNED)
            result = v1->s64 > *((const uint64_t *)v2) ? 1 : -1; 
        else
            result = v1->s64 > *((const double *)v2) ? 1 : -1; 
    }
    else
        result = cmp(v1->v, v2);

    return result;
}

static __kylin_inline void *kmath_val_get(kmath_val_t *val, kobj_t type)
{
    switch(type) {
        case KOBJ_INT8:
        case KOBJ_INT16:
        case KOBJ_INT32:
        case KOBJ_INT64:
            return (void *)&val->s64;
        case KOBJ_UINT8:
        case KOBJ_UINT16:
        case KOBJ_UINT32:
        case KOBJ_UINT64:
            return (void *)&val->u64;
        case KOBJ_STRING:
        case KOBJ_KMAP:
        case KOBJ_KSET:
        case KOBJ_KSTR:
        case KOBJ_OTHERS:
            return val ? val->v : NULL;
        default:
            return val ? val->v : NULL;
    }
    return NULL;
}

/*
 * hash表的通用表现形式
 */
#define KMATH_HASHTABLE_PROTOTYPE(node, type)                                 \
typedef struct {                                                              \
    node        **table;                                                      \
    size_t        cap;                                                        \
    size_t        mask;                                                       \
    size_t        used;                                                       \
    size_t        min;                                                        \
    size_t        max;                                                        \
} type;                                                                       \
                                                                              \
static __kylin_inline void kmath_ht_reset(type *ht)                           \
{                                                                             \
    ht->table = NULL;                                                         \
    ht->cap   = 0;                                                            \
    ht->mask  = 0;                                                            \
    ht->used  = 0;                                                            \
}

#endif /*_KYLIN_MATH_NORMAL_H_*/
