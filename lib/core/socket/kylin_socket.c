#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>

#include <kylin/lib/core/socket/kylin_socket_plugin.h>

struct kylin_socket {
    uint32_t     id;
    ksock_type_t type;
    ksock_opts_t opts
    void        *priv;  /*每个套接字的私有数据，由create生成，destroy销毁*/
};

static uint32_t      sid = 0;
static klist_t      *slist = NULL;  /*list of ksock_t*/

ksock_t *kylin_socket_create(ksock_type_t type, const ksock_opts_t *opts)
{
    ksock_t *guard = NULL;

    if(splugin[type].type == KYLIN_SOCK_MAX)
        return NULL;

    guard = malloc(sizeof(ksock_t));
    if(!guard)
        return NULL;

    guard->id   = eid++;
    guard->type = type;
    memcpy(&guard->opts, opts, sizeof(ksock_opts_t));

    if(splugin[type].reg.create) {
        guard->priv = eplugin[type].reg.create(guard); 
        if(!guard->priv) {
            free(guard);
            return NULL;
        }
    }
    else
        guard->priv = NULL;

    if(!kylin_list_insert_head(slist, guard)) {
        if(splugin[type].reg.destroy)
            splugin[type].reg.destroy(guard->priv);
        free(guard);
        return NULL;
    }

    splugin[type].ref++;

    return guard;
}

void kylin_socket_destroy(ksock_t *guard)
{
    if(!kylin_list_unlink(slist, guard))
        return;

    if(splugin[guard->type].reg.destroy)
        splugin[guard->type].reg.destroy(guard->priv);

    splugin[guard->type].ref--;
    free(guard);

    return;
}

ksock_conn_t *kylin_socket_accept(ksock_t *guard)
{
    ksock_conn_t *conn = NULL;

    if(splugin[guard->type].type == KYLIN_SOCK_MAX)
        return NULL;

    conn = splugin[guard->type].reg.accept(guard);
    if(!conn)
        return NULL;

    return conn;
}

void kylin_socket_connection_destroy(ksock_t *guard, ksock_conn_t *conn)
{
    if(splugin[guard->type].type == KYLIN_SOCK_MAX)
        return;

    splugin[guard->type].reg.conn_destroy(guard, conn);
}

kerr_t kylin_socket_connect(ksock_t *guard)
{
    if(splugin[guard->type].type == KYLIN_SOCK_MAX)
        return KYLIN_ERROR_NOENT;

    return splugin[guard->type].reg.connect(guard);
}

ssize_t kylin_socket_recv(ksock_t *guard, kfd_t fd, void *buf, size_t len)
{
    if(splugin[guard->type].type == KYLIN_SOCK_MAX)
        return KYLIN_ERROR_NOENT;

    return splugin[guard->type].reg.recv(guard, fd, buf, len);
}

ssize_t kylin_socket_send(ksock_t *guard, kfd_t fd, const void *buf, size_t len)
{
    if(splugin[guard->type].type == KYLIN_SOCK_MAX)
        return KYLIN_ERROR_NOENT;

    return splugin[guard->type].reg.send(guard, fd, buf, len);
}

void *kylin_socket_get_priv(ksock_t *guard)
{
    return guard->priv;
}

ksock_type_t kylin_socket_get_type(ksock_t *guard)
{
    return guard->type;
}

ksock_opts_t *kylin_socket_get_opts(ksock_t *guard)
{
    return &guard->opts;
}

kerr_t kylin_socket_init(void)
{
    klist_opts_t slist_opts = {
        .name      = "socket_list",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(ksock_t),
        .match     = __slist_match,
        .allocator = {
            .val_ctor   = NULL, /*值所在的内存由调用者分配*/
            .val_dtor   = NULL,
            .node_ctor  = NULL,
            .node_dtor  = NULL, 
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    slist = kylin_list_create(&slist_opts);
    if(!slist)
        return KYLIN_ERROR_NOMEM;

    return kylin_socket_plugin_init();
}

void kylin_socket_fini(void)
{
    klist_node_t *node  = NULL;
    ksock_t      *guard = NULL;

    if(slist) {
        KYLIN_LIST_FOREACH(slist, node) {
            guard = (ksock_t *)kylin_list_val(slist, node);
            if(!guard)
                continue;

            if(splugin[guard->type].reg.destroy)
                splugin[guard->type].reg.destroy(guard->priv);

            splugin[guard->type].ref--;
        }
        kylin_list_destroy(slist);
    }

    kylin_socket_plugin_fini();
}

