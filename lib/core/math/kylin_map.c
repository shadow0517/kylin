#include <kylin/include/kylin.h>
//#include <kylin/include/arch/kylin_bitops.h>
#include <kylin/include/arch/x86/kylin_bitops.h>
#include <kylin/include/math/kylin_map.h>

#include <kylin/lib/core/math/kylin_math.h>

KMATH_HASHTABLE_PROTOTYPE(kmap_node_t, kmap_ht_t);

struct kylin_map_node {
    kmath_val_t  key;

    kmath_val_t  val;
    kobj_t       val_type;
    size_t       val_size;

    kmap_node_t *next;
};

struct kylin_map {
    kmap_opts_t opts;
    kmap_ht_t   ht[2];
    int32_t     rehashidx;
};

static unsigned int force_resize_ratio = 5;

#define MAP_IS_REHASHING(guard) ((guard)->rehashidx != -1)

#define MAP_PRIV_DATA(guard)    ((guard)->opts.priv)
#define MAP_KEY_TYPE(guard)     ((guard)->opts.key_type)
#define MAP_HASH_FUNC(guard)    ((guard)->opts.hash)
#define MAP_COMP_FUNC(guard)    ((guard)->opts.key_cmp)

#define MAP_KEY_MALLOC(opts)    ((opts)->allocator.key_ctor)
#define MAP_KEY_FREE(opts)      ((opts)->allocator.key_dtor   ? (opts)->allocator.key_dtor   : free)
#define MAP_VAL_MALLOC(opts)    ((opts)->allocator.val_ctor)
#define MAP_VAL_FREE(opts)      ((opts)->allocator.val_dtor   ? (opts)->allocator.val_dtor   : free)
#define MAP_NODE_MALLOC(opts)   ((opts)->allocator.node_ctor  ? (opts)->allocator.node_ctor  : malloc)
#define MAP_NODE_FREE(opts)     ((opts)->allocator.node_dtor  ? (opts)->allocator.node_dtor  : free)
#define MAP_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : malloc)
#define MAP_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : free)

static __kylin_inline void _ht_clear(kmap_t *guard, kmap_ht_t *ht)
{
    kmap_node_t *node = NULL, *next_node = NULL;

    for(size_t i = 0; i < ht->cap && ht->used; i++) {
        node = ht->table[i];
        if(!node)
            continue;

        while(node) {
            next_node = node->next;

            kmath_val_dtor(&node->key, guard->opts.key_type, MAP_KEY_FREE(&guard->opts));
            kmath_val_dtor(&node->val, node->val_type, MAP_VAL_FREE(&guard->opts));
            MAP_NODE_FREE(&guard->opts)(node);
            ht->used--;

            node = next_node;
        }
    }
    return ;
}

static __kylin_inline kbool_t _ht_need_expand(kmap_t *guard)
{
    if(guard->ht[0].cap == 0 ||
            guard->ht[0].used / guard->ht[0].cap > force_resize_ratio)
        return KYLIN_TRUE;
    return KYLIN_FALSE;
}

static __kylin_inline void _map_rehash(kmap_t *guard, int step)
{
    size_t index;
    kmap_node_t *node = NULL, *next_node = NULL, *cmp = NULL;

    if(!MAP_IS_REHASHING(guard))
        return ;

    while(step-- && guard->ht[0].used != 0) {
        while(guard->ht[0].table[guard->rehashidx] == NULL)
            guard->rehashidx++;

        node = guard->ht[0].table[guard->rehashidx];

        while(node) {
            next_node = node->next;

            index = MAP_HASH_FUNC(guard)(kmath_val_get(&node->key, guard->opts.key_type)) & guard->ht[1].mask;

            cmp = guard->ht[1].table[index];
            while(cmp) {
                if(kmath_val_cmp(&node->key, &cmp->key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) {
                    /*node exist, destroy it*/
                    kmath_val_dtor(&node->key, guard->opts.key_type, MAP_KEY_FREE(&guard->opts));
                    kmath_val_dtor(&node->val, node->val_type, MAP_VAL_FREE(&guard->opts));
                    MAP_NODE_FREE(&guard->opts)(node);
                    goto next;
                }
                cmp = cmp->next;
            }

            node->next = guard->ht[1].table[index];
            guard->ht[1].table[index] = node;
            guard->ht[0].used--;
            guard->ht[1].used++;

next:
            node = next_node;
        }
        guard->ht[0].table[guard->rehashidx] = NULL;
        guard->rehashidx++;
    }

    if(guard->ht[0].used == 0) {
        free(guard->ht[0].table);
        guard->ht[0] = guard->ht[1];
        kmath_ht_reset(&guard->ht[1]);
        guard->rehashidx = -1;
    }
}

kmap_t *kylin_map_create(const kmap_opts_t *opts)
{
    kmap_t *map = NULL;

    map = MAP_GUARD_MALLOC(opts)(sizeof(kmap_t));
    if(!map)
        return NULL;
    memset(map, 0, sizeof(kmap_t));

    memcpy(&map->opts, opts, sizeof(kmap_opts_t));
    map->rehashidx = -1;

    return map;
}

void kylin_map_destroy(kmap_t *guard)
{
    if(guard->ht[0].table) {
        _ht_clear(guard, &guard->ht[0]);
        free(guard->ht[0].table);
        kmath_ht_reset(&guard->ht[0]);
    }

    if(guard->ht[1].table) {
        _ht_clear(guard, &guard->ht[1]);
        free(guard->ht[1].table);
        kmath_ht_reset(&guard->ht[1]);
    }

    MAP_GUARD_FREE(&guard->opts)(guard);
}

void *kylin_map_priv(kmap_t *guard)
{
    return MAP_PRIV_DATA(guard);
}

void *kylin_map_key(kmap_t *guard, kmap_node_t *node)
{
    return kmath_val_get(&node->val, guard->opts.key_type);
}

void *kylin_map_val(kmap_t *guard __kylin_unused, kmap_node_t *node)
{
    return kmath_val_get(&node->val, node->val_type);
}

kobj_t kylin_map_val_type(kmap_t *guard __kylin_unused, kmap_node_t *node)
{
    return node->val_type;
}

size_t kylin_map_count(kmap_t *guard)
{
    return (guard->ht[0].used + guard->ht[1].used);
}

kerr_t kylin_map_resize(kmap_t *guard, size_t count)
{
    kmap_ht_t new_ht;
    size_t real_count = 0;

    if(MAP_IS_REHASHING(guard))
        return KYLIN_ERROR_BUSY;

    real_count = kylin_roundup_pow_of_two(count);
    if(real_count == guard->ht[0].cap)
        return KYLIN_ERROR_EXIST;

    new_ht.cap   = real_count;
    new_ht.mask  = real_count - 1;
    new_ht.used  = 0;
    new_ht.table = malloc(real_count * sizeof(kmap_node_t *));
    memset(new_ht.table, 0, real_count * sizeof(kmap_node_t *));

    if(guard->ht[0].table == NULL)
        guard->ht[0] = new_ht;
    else {
        guard->ht[1] = new_ht;
        guard->rehashidx = 0;
    }

    return KYLIN_ERROR_OK;
}

void kylin_map_clear(kmap_t *guard)
{
    if(guard->ht[0].table)
        _ht_clear(guard, &guard->ht[0]);

    if(guard->ht[1].table)
        _ht_clear(guard, &guard->ht[1]);

    guard->rehashidx = -1;
}

kerr_t kylin_map_remove(kmap_t *guard, void *key)
{
    kmap_node_t *node = NULL;

    node = kylin_map_unlink(guard, key);
    if(!node)
        return KYLIN_ERROR_NODATA;

    kmath_val_dtor(&node->key, guard->opts.key_type, MAP_KEY_FREE(&guard->opts));
    kmath_val_dtor(&node->val, node->val_type, MAP_VAL_FREE(&guard->opts));
    MAP_NODE_FREE(&guard->opts)(node);

    return KYLIN_ERROR_OK;
}

kerr_t kylin_map_remove_raw(kmap_t *guard, kmap_node_t *cmp)
{
    kmap_node_t *node = NULL;

    node = kylin_map_unlink_raw(guard, cmp);
    if(!node)
        return KYLIN_ERROR_NODATA;

    kmath_val_dtor(&node->key, guard->opts.key_type, MAP_KEY_FREE(&guard->opts));
    kmath_val_dtor(&node->val, node->val_type, MAP_VAL_FREE(&guard->opts));
    MAP_NODE_FREE(&guard->opts)(node);

    return KYLIN_ERROR_OK;
}

kmap_node_t *kylin_map_unlink_raw(kmap_t *guard, kmap_node_t *node)
{
    uint64_t hash = 0;
    size_t index = 0;
    kmap_node_t *cmp = NULL, *prev_node = NULL;

    if(MAP_IS_REHASHING(guard))
        _map_rehash(guard, 1);

    hash = MAP_HASH_FUNC(guard)(kmath_val_get(&node->key, guard->opts.key_type));

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        prev_node = NULL;

        index = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[index];
        while(cmp) {
            if(kmath_val_cmp(&node->key, &cmp->key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) {
                if(prev_node)
                    prev_node->next = cmp->next;
                else
                    guard->ht[i].table[index] = cmp->next;

                guard->ht[i].used--;
                return cmp;
            }
            prev_node = cmp;
            cmp = cmp->next;
        }
        if(!MAP_IS_REHASHING(guard))
            break;
    }

    return NULL;
}

kmap_node_t *kylin_map_unlink(kmap_t *guard, void *key)
{
    uint64_t hash = 0;
    size_t index = 0;
    kmap_node_t *cmp = NULL, *prev_node = NULL;

    if(MAP_IS_REHASHING(guard))
        _map_rehash(guard, 1);

    hash = MAP_HASH_FUNC(guard)(key);

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        prev_node = NULL;

        index = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[index];
        while(cmp) {
            if(kmath_val_cmp_raw(&cmp->key, key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) {
                if(prev_node)
                    prev_node->next = cmp->next;
                else
                    guard->ht[i].table[index] = cmp->next;

                guard->ht[i].used--;
                return cmp;
            }
            prev_node = cmp;
            cmp = cmp->next;
        }
        if(!MAP_IS_REHASHING(guard))
            break;
    }

    return NULL;
}

kmap_node_t *kylin_map_insert(kmap_t *guard, void *key, void *val, kobj_t val_type, size_t size)
{
    kmap_node_t *node = NULL;

    node = MAP_NODE_MALLOC(&guard->opts)(sizeof(kmap_node_t));
    if(!node)
        return NULL;

    kmath_val_ctor(&node->key, key, guard->opts.key_type, guard->opts.key_size, MAP_KEY_MALLOC(&guard->opts));

    node->val_type = val_type;
    node->val_size = size;
    kmath_val_ctor(&node->val, val, node->val_type, node->val_size, MAP_VAL_MALLOC(&guard->opts));

    return kylin_map_insert_raw(guard, node);
}

kmap_node_t *kylin_map_insert_raw(kmap_t *guard, kmap_node_t *node)
{
    size_t index = 0;
    kmap_ht_t   *ht;
    kmap_node_t *cmp;

    if(_ht_need_expand(guard)) {
        if(guard->ht[0].cap == 0) /*first time to insert entry after create map*/
            kylin_map_resize(guard, 4);
        else                       /*expand ht[1]<cap> two times of ht[0]<used>*/
            kylin_map_resize(guard, guard->ht[0].used * 2);
    }

    if(MAP_IS_REHASHING(guard)) 
        _map_rehash(guard, 1);

    ht = MAP_IS_REHASHING(guard) ? &guard->ht[1] : &guard->ht[0];
    if(!ht->table)
        return NULL;

    index = MAP_HASH_FUNC(guard)(kmath_val_get(&node->key, guard->opts.key_type)) & ht->mask;

    cmp = ht->table[index];
    while(cmp) {
        if(kmath_val_cmp(&node->key, &cmp->key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) 
            return cmp; /*node exist*/
        cmp = cmp->next;
    }

    node->next = ht->table[index];
    ht->table[index] = node;
    ht->used++;

    return node;
}

kmap_node_t *kylin_map_replace(kmap_t *guard, void *key, void *val, kobj_t val_type, size_t size)
{
    uint64_t hash = 0;
    size_t index = 0;
    kmap_node_t *cmp = NULL;

    if(MAP_IS_REHASHING(guard))
        _map_rehash(guard, 1);

    hash = MAP_HASH_FUNC(guard)(key);

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        index = hash & guard->ht[i].mask;
        cmp = guard->ht[i].table[index];
        while(cmp) {
            if(kmath_val_cmp_raw(&cmp->key, key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) {
                kmath_val_dtor(&cmp->val, cmp->val_type, MAP_VAL_FREE(&guard->opts));
                cmp->val_type = val_type;
                cmp->val_size = size;
                kmath_val_ctor(&cmp->val, val, cmp->val_type, cmp->val_size, MAP_VAL_MALLOC(&guard->opts));
                return cmp;
            }
            cmp = cmp->next;
        }
    }

    return NULL;
}

kmap_node_t *kylin_map_find(kmap_t *guard, void *key)
{
    uint64_t hash = 0;
    size_t index = 0;
    kmap_node_t *cmp = NULL;

    hash = MAP_HASH_FUNC(guard)(key);

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        index = hash & guard->ht[i].mask;

        cmp = guard->ht[i].table[index];
        while(cmp) {
            if(kmath_val_cmp_raw(&cmp->key, key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) 
                return cmp;

            cmp = cmp->next;
        }

        if(!MAP_IS_REHASHING(guard))
            break;
    }

    return NULL;
}

kmap_node_t *kylin_map_first(kmap_t *guard)
{
    kmap_ht_t *ht = NULL;

    ht = &guard->ht[0];
    if(!ht->table)
        return NULL;

    for(int i = 0; i < ht->cap; i++) {
        if(ht->table[i])
            return ht->table[i];
    }
    return NULL;
}

kmap_node_t *kylin_map_last(kmap_t *guard)
{
    kmap_ht_t *ht = NULL;
    kmap_node_t *node = NULL;

    ht = MAP_IS_REHASHING(guard) ? &guard->ht[1] : &guard->ht[0];
    if(!ht->table)
        return NULL;

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
kmap_node_t *kylin_map_next(kmap_t *guard, kmap_node_t *node)
{   
    uint64_t hash;
    size_t index;
    kmap_node_t *cmp = NULL;

    /*下一个结点就在该LIST里*/
    if(node->next)
        return node->next;

    hash = MAP_HASH_FUNC(guard)(kmath_val_get(&node->key, guard->opts.key_type));

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        index = hash & guard->ht[i].mask;
        if(guard->ht[i].table[index] == NULL)
            continue;

        /*结点跨LIST，下一个结点在邻居LIST里*/
        cmp = guard->ht[i].table[index]; 
        while(cmp) {
            if(kmath_val_cmp(&node->key, &cmp->key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) {
                /*always at the end of list*/
                while((++index < guard->ht[i].cap) &&
                        (guard->ht[i].table[index] == NULL))
                    continue;

                if(index == guard->ht[i].cap) {
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
                    return guard->ht[i].table[index];
            }
            cmp = cmp->next;
        }
    }
    return NULL;
}

kmap_node_t *kylin_map_prev(kmap_t *guard, kmap_node_t *node)
{
    uint64_t hash;
    long index;
    kmap_node_t *prev_node = NULL, *cmp = NULL;

    hash = MAP_HASH_FUNC(guard)(kmath_val_get(&node->key, guard->opts.key_type));

    for(int i = 0; i <= 1 && guard->ht[i].table; i++) {
        index = hash & guard->ht[i].mask;
        if(guard->ht[i].table[index] == NULL)
            continue;

        prev_node = NULL;
        cmp = guard->ht[i].table[index];
        while(cmp) {
            if(kmath_val_cmp(&node->key, &cmp->key, guard->opts.key_type, MAP_COMP_FUNC(guard)) == 0) {
                /*上一个结点就在该LIST里*/
                if(prev_node)
                    return prev_node;

                /*结点跨LIST，上一个结点在邻居LIST里*/
                while((--index >= 0) &&
                        (guard->ht[i].table[index] == NULL))
                    continue;

                if(index == -1) {
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
                    cmp = guard->ht[i].table[index];
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

void *kylin_map_fetch_val(kmap_t *guard, void *key)
{
    kmap_node_t *node = NULL;

    node = kylin_map_find(guard, key);
    if(!node)
        return NULL;

    return kmath_val_get(&node->val, node->val_type);
}


