#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_tcp.h>

typedef struct {
    kfd_t    fd;
    klist_t *conns; /*对于流类型套接字，该字段存储所有的连接*/
}ksock_tcp_t;

static int __tcp_connection_match(const void *val, const void *key);

static klist_opts_t conns_opts = {
    .name      = "tcp_connections",
    .val_type  = KOBJ_OTHERS,
    .val_size  = sizeof(ksock_conn_t),
    .match     = __tcp_connection_match,
    .allocator = KLIST_OPTS_ALLOCATOR_NULL
};

void *tcp_create(ksock_t *guard)
{
    int set = 1;
    ksock_tcp_t  *sock_tcp = NULL;
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_TCP &&
            type != KYLIN_SOCK_CLIENT_TCP)
        return NULL;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return NULL;

    sock_tcp = kylin_malloc(sizeof(ksock_tcp_t));
    if(!sock_tcp)
        return NULL;

    sock_tcp->fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_tcp->fd == -1) 
        goto error;

    setsockopt(sock_tcp->fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&set, sizeof(int));
    setsockopt(sock_tcp->fd, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));
    setsockopt(sock_tcp->fd, IPPROTO_TCP, TCP_NODELAY, (void *)&set, sizeof(int));

#if defined(__linux__)
    fcntl(sock_tcp->fd, F_SETFL, fcntl(sock_tcp->fd, F_GETFL, 0) | O_NONBLOCK);
#elif defined(__FreeBSD__)
    int flags = fcntl(sock_tcp->fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sock_tcp->fd, F_SETFL, flags);
#endif

    if(type == KYLIN_SOCK_SERVER_TCP) { /*server socket*/
        if(bind(sock_tcp->fd, 
                    (struct sockaddr *)&opts->config.server.local.addr.in, 
                    opts->config.server.local.len) != 0) 
            goto error;

        if(listen(sock_tcp->fd, opts->config.server.backlog) != 0) 
            goto error;

        sock_tcp->conns = kylin_list_create(&conns_opts);
        if(!sock_tcp->conns) 
            goto error;
    }

    return sock_tcp;

error:
    if(sock_tcp && sock_tcp->fd)
        close(sock_tcp->fd);
    if(sock_tcp)
        kylin_free(sock_tcp);
    return NULL;
}

void tcp_destroy(void *priv)
{
    ksock_tcp_t *sock_tcp = (ksock_tcp_t *)priv;

    if(sock_tcp && sock_tcp->fd) 
        close(sock_tcp->fd);
    if(sock_tcp && sock_tcp->conns)
        kylin_list_destroy(sock_tcp->conns);
    if(sock_tcp)
        kylin_free(sock_tcp);

    return;
}

kerr_t tcp_connect(ksock_t *guard)
{
    ksock_tcp_t  *sock_tcp = NULL;
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_CLIENT_TCP)
        return KYLIN_ERROR_INVAL;

    opts = kylin_socket_get_opts(guard);
    if(!opts || !opts->config.server.local.len) 
        return KYLIN_ERROR_NOENT;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || sock_tcp->fd == -1)
        return KYLIN_ERROR_IO;

    if(connect(sock_tcp->fd, 
            (struct sockaddr *)&opts->config.server.local.addr.in, 
            opts->config.server.local.len) != 0)
        return errno;

    return KYLIN_ERROR_OK;
}

ksock_conn_t *tcp_accept(ksock_t *guard)
{
    int                set = 1;
    kfd_t              fd;
    socklen_t          len;
    struct sockaddr_in client;

    ksock_conn_t      *conn     = NULL;
    ksock_opts_t      *opts     = NULL;
    ksock_tcp_t       *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || sock_tcp->fd == -1)
        return NULL;

    fd = accept(sock_tcp->fd, (struct sockaddr *)&client, &len);
    if(fd == -1)
        return NULL;
    setsockopt(fd, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));

    opts = kylin_socket_get_opts(guard);
    if(!opts || !opts->config.server.local.len) {
        close(fd);
        return NULL;
    }

    conn = kylin_malloc(sizeof(ksock_conn_t));
    if(!conn) {
        close(fd);
        return NULL;
    }

    conn->fd         = fd;
    conn->sock       = guard;
    conn->client.len = len;
    memcpy(&conn->client.addr.in, &client, sizeof(struct sockaddr_in));
    memcpy(&conn->server, &opts->config.server.local, sizeof(ksock_addr_t));

    if(kylin_list_insert_head(sock_tcp->conns, conn) != KYLIN_ERROR_OK) {
        close(conn->fd);
        kylin_free(conn);
        return NULL;
    }

    return conn;
}

ssize_t tcp_recv(ksock_t *guard __kylin_unused, kfd_t fd, void *buf, size_t len)
{
    return recv(fd, buf, len, 0);
}

ssize_t tcp_send(ksock_t *guard __kylin_unused, kfd_t fd, const void *buf, size_t len)
{
    return send(fd, buf, len, 0);
}

uint32_t tcp_conn_count(ksock_t *guard)
{
    ksock_tcp_t *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp)
        return 0;

    return kylin_list_count(sock_tcp->conns);
}

ksock_conn_t *tcp_conn_get_first(ksock_t *guard)
{
    klist_node_t *node     = NULL;
    ksock_tcp_t  *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || !sock_tcp->conns)
        return NULL;

    node = kylin_list_first(sock_tcp->conns);
    if(!node)
        return NULL;

    return (ksock_conn_t *)kylin_list_val(sock_tcp->conns, node);
}

ksock_conn_t *tcp_conn_get_next(ksock_t *guard, ksock_conn_t *conn)
{
    klist_node_t *node     = NULL;
    ksock_tcp_t  *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || !sock_tcp->conns)
        return NULL;

    node = kylin_list_find_raw(sock_tcp->conns, conn);
    if(!node)
        return NULL;

    node = kylin_list_next(sock_tcp->conns, node);
    if(!node)
        return NULL;

    return (ksock_conn_t *)kylin_list_val(sock_tcp->conns, node);
}

void tcp_conn_destroy(ksock_t *guard, ksock_conn_t *conn)
{
    klist_node_t *node = NULL;
    ksock_tcp_t *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || !sock_tcp->conns)
        return;

    node = kylin_list_unlink(sock_tcp->conns, conn);
    if(!node)
        return;

    close(conn->fd);
    kylin_free(conn);
}

kfd_t tcp_get_sockfd(ksock_t *guard)
{
    ksock_tcp_t *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp)
        return -1;

    return sock_tcp->fd;
}

static int __tcp_connection_match(const void *val, const void *key)
{
    const ksock_conn_t *conn1 = val;
    const ksock_conn_t *conn2 = key;

    if(conn1->fd > conn2->fd)
        return 1;
    if(conn1->fd < conn2->fd)
        return -1;
    return 0;
}

kerr_t tcp_init(void)
{
    return KYLIN_ERROR_OK;
}

void tcp_fini(void)
{
    return;
}

