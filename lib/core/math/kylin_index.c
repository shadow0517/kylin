#include <kylin/include/kylin.h>
#include <kylin/include/arch/x86/kylin_bitops.h>
#include <kylin/include/math/kylin_index.h>

#include <kylin/lib/core/math/kylin_math.h>

KMATH_HASHTABLE_PROTOTYPE(kindex_node_t, kindex_ht_t);

struct kylin_index_node {
    kmath_val_t    val;
    kindex_node_t *next;
};

struct kylin_index {
    kindex_opts_t opts;
    kindex_ht_t   ht[2];
    int32_t       rehashidx;
};

static unsigned int force_resize_ratio = 5;

#define INDEX_IS_REHASHING(guard) ((guard)->rehashidx != -1)

#define INDEX_PRIV_DATA(guard)    ((guard)->opts.priv)
#define INDEX_VAL_TYPE(guard)     ((guard)->opts.type)
#define INDEX_HASH_FUNC(guard)    ((guard)->opts.hash)
#define INDEX_COMP_FUNC(guard)    ((guard)->opts.compare)

#define INDEX_VAL_MALLOC(opts)    ((opts)->allocator.val_ctor)
#define INDEX_VAL_FREE(opts)      ((opts)->allocator.val_dtor   ? (opts)->allocator.val_dtor   : kylin_free)
#define INDEX_NODE_MALLOC(opts)   ((opts)->allocator.node_ctor  ? (opts)->allocator.node_ctor  : kylin_malloc)
#define INDEX_NODE_FREE(opts)     ((opts)->allocator.node_dtor  ? (opts)->allocator.node_dtor  : kylin_free)
#define INDEX_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : kylin_malloc)
#define INDEX_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : kylin_free)

static __kylin_inline void _ht_clear(kindex_t *guard, kindex_ht_t *ht)
{
    kindex_node_t *node = NULL, *next_node = NULL;

    for(size_t i = 0; i < ht->cap && ht->used; i++) {
        node = ht->table[i];
        if(!node)
            continue;

        while(node) {
            next_node = node->next;

            kmath_val_dtor(&node->val, guard->opts.val_type, INDEX_VAL_FREE(&guard->opts));
            INDEX_NODE_FREE(&guard->opts)(node);
            ht->used--;

            node = next_node;
        }
    }
    return ;
}

static __kylin_inline kbool_t _ht_need_expand(kindex_t *guard)
{
    if(guard->ht[0].cap == 0 ||
            guard->ht[0].used / guard->ht[0].cap > force_resize_ratio)
        return KYLIN_TRUE;
    return KYLIN_FALSE;
}

static __kylin_inline void _index_rehash(kindex_t *guard, int step)
{
    size_t idx;
    kindex_node_t *node = NULL, *next_node = NULL, *cmp = NULL;

    if(!INDEX_IS_REHASHING(guard))
        return ;

    while(step-- && guard->ht[0].used != 0) {
        while(guard->ht[0].table[guard->rehashidx] == NULL)
            guard->rehashidx++;

        node = guard->ht[0].table[guard->rehashidx];

        while(node) {
            next_node = node->next;

            idx = INDEX_HASH_FUNC(guard)(kmath_val_get(&node->val, guard->opts.val_type)) & guard->ht[1].mask;

            cmp = guard->ht[1].table[idx];
            while(cmp) {
                if(kmath_val_cmp(&node->val, &cmp->val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) {
                    /*node exist, destroy it*/
                    kmath_val_dtor(&node->val, guard->opts.val_type, INDEX_VAL_FREE(&guard->opts));
                    INDEX_NODE_FREE(&guard->opts)(node);
                    goto next;
                }
                cmp = cmp->next;
            }

            node->next = guard->ht[1].table[idx];
            guard->ht[1].table[idx] = node;
            guard->ht[0].used--;
            guard->ht[1].used++;

next:
            node = next_node;
        }
        guard->ht[0].table[guard->rehashidx] = NULL;
        guard->rehashidx++;
    }

    if(guard->ht[0].used == 0) {
        kylin_free(guard->ht[0].table);
        guard->ht[0] = guard->ht[1];
        kmath_ht_reset(&guard->ht[1]);
        guard->rehashidx = -1;
    }
}

kindex_t *kylin_index_create(const kindex_opts_t *opts)
{
    kindex_t *index = NULL;

    index = INDEX_GUARD_MALLOC(opts)(sizeof(kindex_t));
    if(!index) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    memcpy(&index->opts, opts, sizeof(kindex_opts_t));
    index->rehashidx = -1;

    return index;
}

void kylin_index_destroy(kindex_t *guard)
{
    if(guard->ht[0].table) {
        _ht_clear(guard, &guard->ht[0]);
        kylin_free(guard->ht[0].table);
        kmath_ht_reset(&guard->ht[0]);
    }

    if(guard->ht[1].table) {
        _ht_clear(guard, &guard->ht[1]);
        kylin_free(guard->ht[1].table);
        kmath_ht_reset(&guard->ht[1]);
    }

    INDEX_GUARD_FREE(&guard->opts)(guard);
}

void *kylin_index_priv(kindex_t *guard)
{
    return guard->opts.priv;
}

kobj_t kylin_index_val_type(kindex_t *guard)
{
    return guard->opts.val_type;
}

size_t kylin_index_val_size(kindex_t *guard)
{
    return guard->opts.val_size;
}

void *kylin_index_val(kindex_t *guard, kindex_node_t *node)
{
    return kmath_val_get(&node->val, guard->opts.val_type);
}

size_t kylin_index_count(kindex_t *guard)
{
    return (guard->ht[0].used + guard->ht[1].used);
}

kerr_t kylin_index_resize(kindex_t *guard, size_t count)
{
    kindex_ht_t new_ht;
    size_t real_count = 0;

    if(INDEX_IS_REHASHING(guard))
        return KYLIN_ERROR_BUSY;

    real_count = kylin_roundup_pow_of_two(count);
    if(real_count == guard->ht[0].cap)
        return KYLIN_ERROR_EXIST;

    new_ht.cap   = real_count;
    new_ht.mask  = real_count - 1;
    new_ht.used  = 0;
    new_ht.table = kylin_malloc(real_count * sizeof(kindex_node_t *));

    if(guard->ht[0].table == NULL)
        guard->ht[0] = new_ht;
    else {
        guard->ht[1] = new_ht;
        guard->rehashidx = 0;
    }

    return KYLIN_ERROR_OK;
}

void kylin_index_clear(kindex_t *guard)
{
    if(guard->ht[0].table)
        _ht_clear(guard, &guard->ht[0]);
    if(guard->ht[1].table)
        _ht_clear(guard, &guard->ht[1]);

    guard->rehashidx = -1;
}

kerr_t kylin_index_remove_raw(kindex_t *guard, kindex_node_t *cmp)
{
    kindex_node_t *node = NULL;

    node = kylin_index_unlink_raw(guard, cmp);
    if(!node)
        return KYLIN_ERROR_NODATA;

    kmath_val_dtor(&node->val, guard->opts.val_type, INDEX_VAL_FREE(&guard->opts));
    INDEX_NODE_FREE(&guard->opts)(node);

    return KYLIN_ERROR_OK;
}

kerr_t kylin_index_remove(kindex_t *guard, void *cmp)
{
    kindex_node_t *node = NULL;

    node = kylin_index_unlink(guard, cmp);
    if(!node)
        return KYLIN_ERROR_NODATA;

    kmath_val_dtor(&node->val, guard->opts.val_type, INDEX_VAL_FREE(&guard->opts));
    INDEX_NODE_FREE(&guard->opts)(node);

    return KYLIN_ERROR_OK;
}

kindex_node_t *kylin_index_unlink_raw(kindex_t *guard, kindex_node_t *node)
{
    uint64_t hash = 0;
    size_t idx = 0;
    kindex_node_t *cmp = NULL, *prev_node = NULL;

    if(INDEX_IS_REHASHING(guard))
        _index_rehash(guard, 1);

    hash = INDEX_HASH_FUNC(guard)(kmath_val_get(&node->val, guard->opts.val_type));

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        prev_node = NULL;

        idx = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[idx];
        while(cmp) {
            if(kmath_val_cmp(&node->val, &cmp->val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) {
                if(prev_node)
                    prev_node->next = cmp->next;
                else
                    guard->ht[i].table[idx] = cmp->next;

                guard->ht[i].used--;
                return cmp;
            }
            prev_node = cmp;
            cmp = cmp->next;
        }
        if(!INDEX_IS_REHASHING(guard))
            break;
    }

    return NULL;
}

void *kylin_index_unlink(kindex_t *guard, void *val)
{
    uint64_t hash = 0;
    size_t idx = 0;
    void *result = NULL;
    kindex_node_t *cmp = NULL, *prev_node = NULL;

    if(INDEX_IS_REHASHING(guard))
        _index_rehash(guard, 1);

    hash = INDEX_HASH_FUNC(guard)(val);

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        prev_node = NULL;

        idx = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[idx];
        while(cmp) {
            if(kmath_val_cmp_raw(&cmp->val, val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) {
                if(prev_node)
                    prev_node->next = cmp->next;
                else
                    guard->ht[i].table[idx] = cmp->next;

                guard->ht[i].used--;

                result = kmath_val_get(&cmp->val, guard->opts.val_type);
                INDEX_NODE_FREE(&guard->opts)(cmp);
                return result;
            }
            prev_node = cmp;
            cmp = cmp->next;
        }
        if(!INDEX_IS_REHASHING(guard))
            break;
    }

    return NULL;
}

void *kylin_index_insert(kindex_t *guard, void *val)
{
    kindex_node_t *node = NULL;

    node = INDEX_NODE_MALLOC(&guard->opts)(sizeof(kindex_node_t));
    if(!node) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    kmath_val_ctor(&node->val, val, guard->opts.val_type, guard->opts.val_size, INDEX_VAL_MALLOC(&guard->opts));

    node = kylin_index_insert_raw(guard, node);
    if(!node) {
        kmath_val_dtor(&node->val, guard->opts.val_type, INDEX_VAL_FREE(&guard->opts));
        INDEX_NODE_FREE(&guard->opts)(node);
        return NULL;
    }

    return kmath_val_get(&node->val, guard->opts.val_type);
}

kindex_node_t *kylin_index_insert_raw(kindex_t *guard, kindex_node_t *node)
{
    size_t idx = 0;
    kindex_ht_t   *ht;
    kindex_node_t *cmp;

    if(_ht_need_expand(guard)) {
        if(guard->ht[0].cap == 0) /*first time to insert entry after create index*/
            kylin_index_resize(guard, 4);
        else                       /*expand ht[1]<cap> two times of ht[0]<used>*/
            kylin_index_resize(guard, guard->ht[0].used * 2);
    }

    if(INDEX_IS_REHASHING(guard)) 
        _index_rehash(guard, 1);

    ht = INDEX_IS_REHASHING(guard) ? &guard->ht[1] : &guard->ht[0];
    if(!ht->table) {
        kerrno = KYLIN_ERROR_NOENT;
        return NULL;
    }

    idx = INDEX_HASH_FUNC(guard)(kmath_val_get(&node->val, guard->opts.val_type)) & ht->mask;
    cmp = ht->table[idx];

    while(cmp) {
        if(kmath_val_cmp(&node->val, &cmp->val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) 
            return cmp; /*node exist*/
        cmp = cmp->next;
    }

    node->next = ht->table[idx];
    ht->table[idx] = node;
    ht->used++;

    return node;
}

kindex_node_t *kylin_index_replace(kindex_t *guard, void *val)
{
    uint64_t hash = 0;
    size_t idx = 0;
    kindex_node_t *cmp = NULL;

    if(INDEX_IS_REHASHING(guard))
        _index_rehash(guard, 1);

    hash = INDEX_HASH_FUNC(guard)(val);

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        idx = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[idx];
        while(cmp) {
            if(kmath_val_cmp_raw(&cmp->val, val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) {
                kmath_val_dtor(&cmp->val, guard->opts.val_type, INDEX_VAL_FREE(&guard->opts));
                kmath_val_ctor(&cmp->val, val, guard->opts.val_type, guard->opts.val_size, INDEX_VAL_MALLOC(&guard->opts));
                return cmp;
            }
            cmp = cmp->next;
        }
    }

    return NULL;
}

kindex_node_t *kylin_index_find(kindex_t *guard, void *val)
{
    uint64_t hash = 0;
    size_t idx = 0;
    kindex_node_t *cmp = NULL;

    hash = INDEX_HASH_FUNC(guard)(val);

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        idx = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[idx];

        while(cmp) {
            if(kmath_val_cmp_raw(&cmp->val, val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) 
                return cmp;

            cmp = cmp->next;
        }

        if(!INDEX_IS_REHASHING(guard))
            break;
    }

    return NULL;
}

kindex_node_t *kylin_index_first(kindex_t *guard)
{
    kindex_ht_t *ht = NULL;

    ht = &guard->ht[0];
    if(!ht->table) {
        kerrno = KYLIN_ERROR_NOENT;
        return NULL;
    }

    for(int i = 0; i < ht->cap; i++) {
        if(ht->table[i])
            return ht->table[i];
    }

    return NULL;
}

kindex_node_t *kylin_index_last(kindex_t *guard)
{
    kindex_ht_t *ht = NULL;
    kindex_node_t *node = NULL;

    ht = INDEX_IS_REHASHING(guard) ? &guard->ht[1] : &guard->ht[0];
    if(!ht->table) {
        kerrno = KYLIN_ERROR_NOENT;
        return NULL;
    }

    for(int i = (ht->cap - 1); i >= 0; i--) {
        if(ht->table[i]) {
            node = ht->table[i];
            while(node->next)
                node = node->next;
            return node;
        }
    }
    return NULL;
}

/*next和prev操作要处理结点 <跨LIST> 和 <跨HT> 的特殊情况*/
kindex_node_t *kylin_index_next(kindex_t *guard, kindex_node_t *node)
{
    uint64_t hash;
    size_t idx;
    kindex_node_t *cmp = NULL;

    /*下一个结点就在该LIST里*/
    if(node->next)
        return node->next;

    hash = INDEX_HASH_FUNC(guard)(kmath_val_get(&node->val, guard->opts.val_type));

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        idx = hash & guard->ht[i].mask;
        if(guard->ht[i].table[idx] == NULL)
            continue;

        /*结点跨LIST，下一个结点在邻居LIST里*/
        cmp = guard->ht[i].table[idx]; 
        while(cmp) {
            if(kmath_val_cmp(&node->val, &cmp->val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) {
                /*always at the end of list*/
                while((++idx < guard->ht[i].cap) &&
                        (guard->ht[i].table[idx] == NULL))
                    continue;

                if(idx == guard->ht[i].cap) {
                    if(i == 1) /*node为INDEX的最后一个结点*/
                        return NULL;
                    else {
                        /*结点跨HT，下一个结点在HT[1]里*/
                        if(!guard->ht[1].table)
                            return NULL; /*node为INDEX的最后一个结点*/
                        for(int j = 0; j < guard->ht[1].cap; j++) {
                            if(guard->ht[1].table[j] != NULL)
                                return guard->ht[1].table[j];
                        }
                    }
                }
                else
                    return guard->ht[i].table[idx];
            }
            cmp = cmp->next;
        }
    }
    return NULL;
}

kindex_node_t *kylin_index_prev(kindex_t *guard, kindex_node_t *node)
{
    uint64_t hash;
    long idx;
    kindex_node_t *prev_node = NULL, *cmp = NULL;

    hash = INDEX_HASH_FUNC(guard)(kmath_val_get(&node->val, guard->opts.val_type));

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        idx = hash & guard->ht[i].mask;
        if(guard->ht[i].table[idx] == NULL)
            continue;

        prev_node = NULL;
        cmp = guard->ht[i].table[idx];
        while(cmp) {
            if(kmath_val_cmp(&node->val, &cmp->val, guard->opts.val_type, INDEX_COMP_FUNC(guard)) == 0) {
                /*上一个结点就在该LIST里*/
                if(prev_node)
                    return prev_node;

                /*结点跨LIST，上一个结点在邻居LIST里*/
                while((--idx >= 0) &&
                        (guard->ht[i].table[idx] == NULL))
                    continue;

                if(idx == -1) {
                    if(i == 0) /*node为INDEX的第一个结点*/
                        return NULL;
                    else {
                        /*结点跨HT，上一个结点在HT[0]里*/
                        if(!guard->ht[0].table)
                            return NULL; /*node为INDEX的第一个结点*/
                        for(int j = (guard->ht[0].cap - 1); j >= 0; j--) {
                            if(guard->ht[0].table[j] != NULL) {
                                cmp = guard->ht[0].table[j];
                                while(cmp->next)
                                    cmp = cmp->next;
                                return cmp;
                            }
                        }
                    }
                }
                else {
                    cmp = guard->ht[i].table[idx];
                    while(cmp->next)
                        cmp = cmp->next;
                    return cmp;
                }
            }
            prev_node = cmp;
            cmp = cmp->next;
        }
    }
    return NULL;
}

