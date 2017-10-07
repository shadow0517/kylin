#ifndef _KYLIN_MATH_LIST_H_
#define _KYLIN_MATH_LIST_H_

#include <kylin/include/utils/kylin_object.h>

/*
 * 列表
 */
struct kylin_list;
struct kylin_list_node;
/*struct kylin_list_iterator;*/
typedef struct kylin_list          klist_t;
typedef struct kylin_list_node     klist_node_t;
/*typedef struct kylin_list_iterator klist_iter_t;*/

typedef struct {
    const char *name;
    kobj_t      val_type;
    size_t      val_size;

    int (*match)(void *val, void *key);

    struct {
        void *(*val_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*val_dtor)(void *);
        void *(*node_ctor)(size_t);
        void  (*node_dtor)(void *);
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} klist_opts_t;

extern klist_t *kylin_list_create(const klist_opts_t *opts);
extern void kylin_list_destroy(klist_t *);
extern void kylin_list_clear(klist_t *);

extern klist_t *kylin_list_dup(const klist_t *);

extern size_t kylin_list_count(const klist_t *);
extern void *kylin_list_val(const klist_t *, klist_node_t *);

extern klist_node_t *kylin_list_first(const klist_t *);
extern klist_node_t *kylin_list_last(const klist_t *);
extern klist_node_t *kylin_list_prev(const klist_node_t *);
extern klist_node_t *kylin_list_next(const klist_node_t *);

extern klist_node_t *kylin_list_insert_head(klist_t *, void *);
extern klist_node_t *kylin_list_insert_tail(klist_t *, void *);
extern klist_node_t *kylin_list_insert(klist_t *, klist_node_t *before, void *);

extern klist_node_t *kylin_list_insert_head_raw(klist_t *, klist_node_t *);
extern klist_node_t *kylin_list_insert_tail_raw(klist_t *, klist_node_t *);
extern klist_node_t *kylin_list_insert_raw(klist_t *, klist_node_t *before, klist_node_t *);

extern void kylin_list_remove(klist_t *, void *);
extern void kylin_list_remove_raw(klist_t *, klist_node_t *);

extern klist_node_t *kylin_list_unlink(klist_t *, void *);
extern klist_node_t *kylin_list_unlink_raw(klist_t *, klist_node_t *);

extern klist_node_t *kylin_list_find(const klist_t *, void *key);
extern klist_node_t *kylin_list_find_index(const klist_t *, int index);

extern void kylin_list_rotate(klist_t *);
extern void kylin_list_join(klist_t *, klist_t *);

#endif /*_KYLIN_MATH_LIST_H_*/
