#include <kylin/include/kylin.h>
#include <kylin/include/kylin_log.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/log/kylin_log_obj.h>

static int __obj_compare(const void *c1, const void *c2)
{
    const klog_t *comp1 = c1;
    const klog_t *comp2 = c2;

    if(comp1->id > comp2->id)
        return 1;
    if(comp1->id < comp2->id)
        return -1;
    return 0;
}

static krb_opts_t log_obj_opts = {
    .name      = "log_object",
    .val_type  = KOBJ_OTHERS,
    .val_size  = sizeof(klog_t),
    .compare   = __obj_compare,
    .allocator = {
        .val_ctor   = NULL, /*值所在的内存由调用者分配*/
        .val_dtor   = NULL,
        .node_ctor  = NULL,
        .node_dtor  = NULL,
        .guard_ctor = NULL,
        .guard_dtor = NULL
    }
};

static krb_t *obj_rb = NULL;

kerr_t object_insert(klog_t *obj)
{
    krb_node_t *node = NULL;

    node = kylin_rb_insert(obj_rb, obj);
    if(!node)
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

kerr_t object_remove(uint32_t id)
{
    klog_t cmp;
    krb_node_t *node = NULL;

    memset(&cmp, 0, sizeof(klog_t));
    cmp.id = id;

    node = kylin_rb_find(obj_rb, &cmp);
    if(!node)
        return KYLIN_ERROR_NOENT;

    kylin_rb_remove(obj_rb, node);

    return KYLIN_ERROR_OK;
}

klog_t *object_find(uint32_t id)
{
    klog_t cmp;
    krb_node_t *node = NULL;

    memset(&cmp, 0, sizeof(klog_t));
    cmp.id = id;

    node = kylin_rb_find(obj_rb, &cmp);
    if(!node)
        return NULL;

    return (klog_t *)kylin_rb_val(obj_rb, node);
}

kerr_t object_init(void)
{
    obj_rb = kylin_rb_create(&log_obj_opts);
    if(!obj_rb)
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

void object_fini(void)
{
    if(obj_rb)
        kylin_rb_destroy(obj_rb);
}

