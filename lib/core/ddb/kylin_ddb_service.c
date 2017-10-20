#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/math/kylin_list.h>

#include <kylin/include/kylin_ddb.h>
#include <kylin/lib/core/ddb/kylin_ddb_service.h>

static klist_t *service_list = NULL;

kerr_t kylin_ddb_service_register(kddb_service_t *service)
{
    if(!kylin_list_insert_head(service_list, service))
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

void kylin_ddb_service_unregister(kddb_service_t *service)
{
    kylin_list_remove(service_list, service);
}

kddb_service_t *service_get_first(void)
{
    klist_node_t   *node    = NULL;

    node = kylin_list_first(service_list);
    if(!node)
        return NULL;

    return (kddb_service_t *)kylin_list_val(service_list, node);
}

kddb_service_t *service_get_next(kddb_service_t *service)
{
    klist_node_t   *node    = NULL;

    node = kylin_list_find_raw(service_list, service);
    if(!node)
        return NULL;

    node = kylin_list_next(service_list, node);
    if(!node)
        return NULL;

    return (kddb_service_t *)kylin_list_val(service_list, node);
}

static int __service_match(const void *val, const void *key)
{
    const kddb_service_t *s1 = val;
    const kddb_service_t *s2 = key;

    if(s1->exec > s2->exec)
        return 1;
    if(s1->exec < s2->exec)
        return -1;
    return 0;
}

kerr_t service_init(void)
{
    klist_opts_t service_opts = {
        .name      = "ddb service",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kddb_service_t),
        .match     = __service_match,
        .allocator = KLIST_OPTS_ALLOCATOR_VAL(malloc)
    };

    service_list = kylin_list_create(&service_opts);
    if(!service_list)
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

void service_fini(void)
{
    if(service_list)
        kylin_list_destroy(service_list);

    return;
}

