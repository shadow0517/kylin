#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_list.h>

#include <kylin/lib/core/math/kylin_math.h>

struct kylin_list_node {
    klist_node_t *prev;
    klist_node_t *next;
    kmath_val_t   val;
};

/*
 * 哨兵
 */
struct kylin_list {
    klist_node_t *head;
    klist_node_t *tail;
    klist_opts_t  opts;
    size_t        count;
};

#define LIST_HEAD(guard)   ((guard)->head)
#define LIST_TAIL(guard)   ((guard)->tail)
#define LIST_COUNT(guard)  ((guard)->count)

#define LIST_NEXT(node)  ((node)->next)
#define LIST_PREV(node)  ((node)->prev)
#define LIST_VAL(node)   ((node)->val)

#define LIST_VAL_MATCH(opts)     ((opts)->match)
#define LIST_VAL_MALLOC(opts)    ((opts)->allocator.val_ctor)
#define LIST_VAL_FREE(opts)      ((opts)->allocator.val_dtor   ? (opts)->allocator.val_dtor   : kylin_free)
#define LIST_NODE_MALLOC(opts)   ((opts)->allocator.node_ctor  ? (opts)->allocator.node_ctor  : kylin_malloc)
#define LIST_NODE_FREE(opts)     ((opts)->allocator.node_dtor  ? (opts)->allocator.node_dtor  : kylin_free)
#define LIST_GUARD_MALLOC(opts)  ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : kylin_malloc)
#define LIST_GUARD_FREE(opts)    ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : kylin_free)

klist_t *kylin_list_create(const klist_opts_t *opts)
{
    klist_t *guard = NULL;

    guard = LIST_GUARD_MALLOC(opts)(sizeof(klist_t));
    if(!guard) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    memcpy(&guard->opts, opts, sizeof(klist_opts_t));

    return guard;
}

void kylin_list_destroy(klist_t *guard)
{
    kylin_list_clear(guard);
    LIST_GUARD_FREE(&guard->opts)(guard);
}

void kylin_list_clear(klist_t *guard)
{
    klist_node_t *node = NULL;

    while((node = LIST_HEAD(guard)) != NULL) {
        kmath_val_dtor(&LIST_VAL(node), guard->opts.val_type, LIST_VAL_FREE(&guard->opts));
        kylin_list_remove(guard, node);
    }
}

klist_t *kylin_list_dup(const klist_t *guard)
{
    klist_t *dup = NULL;
    klist_node_t *tmp = NULL;

    dup = kylin_list_create(&guard->opts);
    if(!dup)
        return NULL;

    for(tmp = LIST_HEAD(guard); tmp != NULL; tmp = LIST_NEXT(tmp)) {
        if(NULL != kylin_list_insert_tail(dup, tmp)) {
            kylin_list_destroy(dup);
            return NULL;
        }
    }

    return dup;
}

size_t kylin_list_count(const klist_t *guard)
{
    return LIST_COUNT(guard);
}

void *kylin_list_val(const klist_t *guard, klist_node_t *node)
{
    return kmath_val_get(&LIST_VAL(node), guard->opts.val_type);
}

klist_node_t *kylin_list_first(const klist_t *guard)
{
    return LIST_HEAD(guard);
}

klist_node_t *kylin_list_last(const klist_t *guard)
{
    return LIST_TAIL(guard);
}

klist_node_t *kylin_list_prev(const klist_node_t *node)
{
    return LIST_PREV(node);
}

klist_node_t *kylin_list_next(const klist_node_t *node)
{
    return LIST_NEXT(node);
}

klist_node_t *kylin_list_insert_head(klist_t *guard, void *val)
{
    klist_node_t *node = NULL;

    node = LIST_NODE_MALLOC(&guard->opts)(sizeof(klist_node_t));
    if(!node) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    kmath_val_ctor(&LIST_VAL(node), val, guard->opts.val_type, 
            guard->opts.val_size, LIST_VAL_MALLOC(&guard->opts));

    if(LIST_COUNT(guard) == 0) {
        LIST_HEAD(guard) = LIST_TAIL(guard) = node;
        LIST_PREV(node) = LIST_NEXT(node) = NULL;
    }
    else {
        LIST_PREV(node) = NULL;
        LIST_NEXT(node) = LIST_HEAD(guard);
        LIST_PREV(LIST_HEAD(guard)) = node;
        LIST_HEAD(guard) = node;
    }
    LIST_COUNT(guard)++;

    return node;
}

klist_node_t *kylin_list_insert_tail(klist_t *guard, void *val)
{
    klist_node_t *node = NULL;

    node = LIST_NODE_MALLOC(&guard->opts)(sizeof(klist_node_t));
    if(!node) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    kmath_val_ctor(&LIST_VAL(node), val, guard->opts.val_type, 
            guard->opts.val_size, LIST_VAL_MALLOC(&guard->opts));

    if(LIST_COUNT(guard) == 0) {
        LIST_HEAD(guard) = LIST_TAIL(guard) = node;
        LIST_PREV(node) = LIST_NEXT(node) = NULL;
    }
    else {
        LIST_PREV(node) = LIST_TAIL(guard);
        LIST_NEXT(node) = NULL;
        LIST_NEXT(LIST_TAIL(guard)) = node;
        LIST_TAIL(guard) = node;
    }
    LIST_COUNT(guard)++;

    return node;
}

klist_node_t *kylin_list_insert(klist_t *guard, klist_node_t *before, void *val)
{
    klist_node_t *node = NULL;

    node = LIST_NODE_MALLOC(&guard->opts)(sizeof(klist_node_t));
    if(!node) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    kmath_val_ctor(&LIST_VAL(node), val, guard->opts.val_type, 
            guard->opts.val_size, LIST_VAL_MALLOC(&guard->opts));

    LIST_PREV(node) = before;
    LIST_NEXT(node) = LIST_NEXT(before);
    if(LIST_TAIL(guard) == before)
        LIST_TAIL(guard) = node;
    if(LIST_PREV(node) != NULL)
        LIST_NEXT(LIST_PREV(node)) = node;
    if(LIST_NEXT(node) != NULL)
        LIST_PREV(LIST_NEXT(node)) = node;
    LIST_COUNT(guard)++;

    return node;
}

klist_node_t *kylin_list_insert_head_raw(klist_t *guard, klist_node_t *node)
{
    return NULL;
}

klist_node_t *kylin_list_insert_tail_raw(klist_t *guard, klist_node_t *node)
{
    return NULL;
}

klist_node_t *kylin_list_insert_raw(klist_t *guard, klist_node_t *before, klist_node_t *node)
{
    return NULL;
}

void kylin_list_remove(klist_t *guard, void *cmp)
{
    return;
}

void kylin_list_remove_raw(klist_t *guard, klist_node_t *node)
{
    if(LIST_PREV(node))
        LIST_NEXT(LIST_PREV(node)) = LIST_NEXT(node);
    else
        LIST_HEAD(guard) = LIST_NEXT(node);

    if(LIST_NEXT(node))
        LIST_PREV(LIST_NEXT(node)) = LIST_PREV(node);
    else
        LIST_TAIL(guard) = LIST_PREV(node);

    LIST_COUNT(guard)--;
    LIST_NODE_FREE(&guard->opts)(node);
}

klist_node_t *kylin_list_unlink(klist_t *guard, void *cmp)
{
    return NULL;
}

klist_node_t *kylin_list_unlink_raw(klist_t *guard, klist_node_t *node)
{
    return NULL;
}

klist_node_t *kylin_list_find(const klist_t *guard, void *key)
{
    klist_node_t *tmp = NULL;

    for(tmp = LIST_HEAD(guard); tmp != NULL; tmp = LIST_NEXT(tmp)) {
        if(LIST_VAL_MATCH(&guard->opts)(LIST_VAL(tmp).v, key) == 0)
            return tmp;
    }

    return NULL;
}

klist_node_t *kylin_list_find_index(const klist_t *guard, int index)
{
    klist_node_t *tmp = LIST_HEAD(guard);

    if(index > LIST_COUNT(guard))
        return NULL;

    for(int i = 1; i < LIST_COUNT(guard); i++) {
        if(i == index)
            return tmp;
        tmp = LIST_NEXT(tmp);
    }

    return NULL;
}

void kylin_list_rotate(klist_t *guard)
{
    klist_node_t *tmp = NULL;
    klist_node_t *tmp_next = NULL;

    if(LIST_COUNT(guard) <= 1)
        return ;

    for(tmp = LIST_HEAD(guard); 
            tmp != NULL && (tmp_next = LIST_NEXT(tmp)) != NULL; 
            tmp = tmp_next)
        KYLIN_SWAP(tmp->prev, tmp->next);

    KYLIN_SWAP(LIST_HEAD(guard), LIST_TAIL(guard));
}

void kylin_list_join(klist_t *dest, klist_t *src)
{
    if(LIST_HEAD(src))
        LIST_PREV(LIST_HEAD(src)) = LIST_TAIL(dest);

    if(LIST_TAIL(dest))
        LIST_NEXT(LIST_TAIL(dest)) = LIST_HEAD(src);
    else
        LIST_HEAD(dest) = LIST_HEAD(src);

    LIST_TAIL(dest) = LIST_TAIL(src);
    LIST_COUNT(dest) += LIST_COUNT(src);

    LIST_HEAD(src) = LIST_TAIL(src) = NULL;
    LIST_COUNT(src) = 0;
}


