#ifndef _KYLIN_MATH_RB_H_
#define _KYLIN_MATH_RB_H_

#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_object.h>
/*
 * kylin/lib/module/basic/kylin_rb.c
 */

struct kylin_rb;
struct kylin_rb_node;
/*struct kylin_rb_iterator;*/
typedef struct kylin_rb          krb_t;
typedef struct kylin_rb_node     krb_node_t;
/*typedef struct kylin_rb_iterator krb_iter_t;*/

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

extern krb_t *kylin_rb_create(const krb_opts_t *opts);
extern void kylin_rb_destroy(krb_t *);
extern void kylin_rb_clear(krb_t *);

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

extern void *kylin_rb_val(const krb_t *, krb_node_t *);
extern size_t kylin_rb_count(krb_t *);

extern krb_node_t *kylin_rb_min(krb_t *);
extern krb_node_t *kylin_rb_max(krb_t *);

extern krb_node_t *kylin_rb_insert(krb_t *, void *val);
extern krb_node_t *kylin_rb_insert_raw(krb_t *, krb_node_t *);

extern void kylin_rb_remove(krb_t *, void *cmp);
extern void kylin_rb_remove_raw(krb_t *, krb_node_t *);

extern krb_node_t *kylin_rb_unlink(krb_t *, void *cmp);
extern krb_node_t *kylin_rb_unlink_raw(krb_t *, krb_node_t *);

extern krb_node_t *kylin_rb_next(krb_node_t *);
extern krb_node_t *kylin_rb_prev(krb_node_t *);
extern krb_node_t *kylin_rb_find(krb_t *, void *cmp);
extern krb_node_t *kylin_rb_find_or_next(krb_t *, void *cmp);

#define kylin_rb_foreach(root, node) \
    for(node = kylin_rb_min(root); node != NULL; node = kylin_rb_next(node))

#endif /*_KYLIN_MATH_RB_H_*/
