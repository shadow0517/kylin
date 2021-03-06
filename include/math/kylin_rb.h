#ifndef _KYLIN_MATH_RB_H_
#define _KYLIN_MATH_RB_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_object.h>

struct kylin_rb;
struct kylin_rb_node;
typedef struct kylin_rb          krb_t;
typedef struct kylin_rb_node     krb_node_t;

typedef struct {
    const char *name;
    kobj_t      val_type;
    size_t      val_size;

    int (*compare)(const void *, const void *);

    /*
     * TODO: 对于配置类的信息，可以采取这种方式替代小型数据库
    struct {
        void    *priv;
        FILE    *path;
        void    *(*parse)(krb_t *, krb_opts_t *, kjson_t *);
        kjson_t *(*build)(krb_t *, krb_opts_t *, void *);
    } json;
    */

    /*
     * TODO:对于大量且需要持久化的数据，可以采用这种方式简化数据的存储
    struct {
        void             *priv;
        kcache_connect_t *conn;
        kerr_t (*insert)(krb_t *, krb_opts_t *, void *);
        kerr_t (*remove)(krb_t *, krb_opts_t *, void *);
        kerr_t (*update)(krb_t *, krb_opts_t *, void *); 
        kerr_t (*upload)(krb_t *, krb_opts_t *);         //把数据从磁盘（数据库）加载到内存，由模块创建（kylin_rb_create）的时候调用
    } cache;
    */

    struct {
        void *(*val_ctor)(size_t); /*如果为NULL，表示插入的值已分配好内存*/
        void  (*val_dtor)(void *);
        void *(*node_ctor)(size_t);
        void  (*node_dtor)(void *);
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} krb_opts_t;

#define KRB_OPTS_ALLOCATOR_NULL     \
{                                   \
    .val_ctor   = NULL,             \
    .val_dtor   = NULL,             \
    .node_ctor  = NULL,             \
    .node_dtor  = NULL,             \
    .guard_ctor = NULL,             \
    .guard_dtor = NULL              \
}

#define KRB_OPTS_ALLOCATOR_VAL(ctor)    \
{                                       \
    .val_ctor   = ctor,                 \
    .val_dtor   = NULL,                 \
    .node_ctor  = NULL,                 \
    .node_dtor  = NULL,                 \
    .guard_ctor = NULL,                 \
    .guard_dtor = NULL                  \
}

extern krb_t *kylin_rb_create(const krb_opts_t *opts) 
    __kylin_nonnull((1));
extern void kylin_rb_destroy(krb_t *) 
    __kylin_nonnull((1));
extern void kylin_rb_clear(krb_t *) 
    __kylin_nonnull((1));

/*
 * TODO: 对于配置类的信息，可以采取这种方式替代小型数据库
 * json_to_struct: json_parse
 * struct_to_json: json_build
extern kjson_t *kylin_rb_dump(krb_t *);
extern kerr_t kylin_rb_load(kjson_t *, kjson_error_t *);
extern kerr_t kylin_rb_dumpf(krb_t *);
extern kerr_t kylin_rb_loadf(krb_t *);
#define KRB_FLASH_JSON  1UL
#define KRB_FLASH_CACHE 2UL
extern kerr_t kylin_rb_flash(krb_t *, kword_t flags);
*/

extern void *kylin_rb_val(const krb_t *, krb_node_t *) 
    __kylin_nonnull((1, 2));
extern size_t kylin_rb_count(krb_t *) 
    __kylin_nonnull((1));

extern void *kylin_rb_insert(krb_t *, void *val) 
    __kylin_nonnull((1, 2));
extern void kylin_rb_insert_raw(krb_t *, krb_node_t *) 
    __kylin_nonnull((1, 2));

extern void kylin_rb_remove(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));
extern void kylin_rb_remove_raw(krb_t *, krb_node_t *) 
    __kylin_nonnull((1, 2));

extern void *kylin_rb_unlink(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));
extern krb_node_t *kylin_rb_unlink_raw(krb_t *, krb_node_t *) 
    __kylin_nonnull((1, 2));

extern krb_node_t *kylin_rb_min(krb_t *) 
    __kylin_nonnull((1));
extern krb_node_t *kylin_rb_max(krb_t *) 
    __kylin_nonnull((1));

extern void *kylin_rb_next(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));
extern void *kylin_rb_prev(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));

extern krb_node_t *kylin_rb_next_raw(krb_t *, krb_node_t *) 
    __kylin_nonnull((1, 2));
extern krb_node_t *kylin_rb_prev_raw(krb_t *, krb_node_t *) 
    __kylin_nonnull((1, 2));

extern void *kylin_rb_find(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));
extern void *kylin_rb_find_or_next(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));

extern krb_node_t *kylin_rb_find_raw(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));
extern krb_node_t *kylin_rb_find_or_next_raw(krb_t *, void *cmp) 
    __kylin_nonnull((1, 2));

#define KYLIN_RB_FOREACH(root, node) \
    for(node = kylin_rb_min(root); node != NULL; node = kylin_rb_next(root, node))

#endif /*_KYLIN_MATH_RB_H_*/
