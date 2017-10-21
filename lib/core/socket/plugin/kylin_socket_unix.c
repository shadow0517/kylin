#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_unix.h>

typedef struct {
    kfd_t    fd;
    klist_t *conns; /*对于流类型套接字，该字段存储所有的连接*/
}ksock_unix_t;

static int __unix_connection_match(const void *val, const void *key);

static klist_opts_t conns_opts = {
    .name      = "unix_connections",
    .val_type  = KOBJ_OTHERS,
    .val_size  = sizeof(ksock_conn_t),
    .match     = __unix_connection_match,
    .allocator = KLIST_OPTS_ALLOCATOR_NULL
};

void *unix_create(ksock_t *guard)
{
    ksock_unix_t *sock_unix = NULL;
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_UNIX &&
            type != KYLIN_SOCK_CLIENT_UNIX)
        return NULL;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return NULL;

    sock_unix = kylin_malloc(sizeof(ksock_unix_t));
    if(!sock_unix)
        return NULL;

    sock_unix->fd = socket(PF_LOCAL, opts->type, 0);
    if(sock_unix->fd == -1) {
        kylin_free(sock_unix);
        return NULL;
    }

    if(type == KYLIN_SOCK_SERVER_UNIX) { /*server socket*/
        /*  Delete any other file that gets in our way  */
        unlink(opts->config.server.local.addr.un.sun_path);

        if(bind(sock_unix->fd, 
                    (struct sockaddr *)&opts->config.server.local.addr.un, 
                    opts->config.server.local.len) != 0) {
            close(sock_unix->fd);
            kylin_free(sock_unix);
            return NULL;
        }

#ifdef SO_NOSIGPIPE
        int set = 1;
        setsockopt(sock_unix->fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif

#if defined(__linux__)
        fcntl(sock_unix->fd, F_SETFL, fcntl(sock_unix->fd, F_GETFL, 0) | O_NONBLOCK);
#elif defined(__FreeBSD__)
        int flags = fcntl(sock_unix->fd, F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(sock_unix->fd, F_SETFL, flags);
#endif

        if(listen(sock_unix->fd, opts->config.server.backlog) != 0) {
            close(sock_unix->fd);
            kylin_free(sock_unix);
            return NULL;
        }

        if(opts->type == SOCK_STREAM) {
            sock_unix->conns = kylin_list_create(&conns_opts);
            if(!sock_unix->conns) {
                close(sock_unix->fd);
                kylin_free(sock_unix);
                return NULL;
            }
        }
    }

    return sock_unix;
}

void unix_destroy(void *priv)
{
    ksock_unix_t *sock_unix = (ksock_unix_t *)priv;

    if(sock_unix) {
        if(sock_unix->fd)
            close(sock_unix->fd);
        if(sock_unix->conns)
            kylin_list_destroy(sock_unix->conns);
        kylin_free(sock_unix);
    }

    return;
}

kerr_t unix_connect(ksock_t *guard)
{
    ksock_unix_t *sock_unix = NULL;
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_CLIENT_UNIX)
        return KYLIN_ERROR_INVAL;

    opts = kylin_socket_get_opts(guard);
    if(!opts || !opts->config.server.local.len) 
        return KYLIN_ERROR_NOENT;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix || sock_unix->fd == -1)
        return KYLIN_ERROR_IO;

    if(connect(sock_unix->fd, 
            (struct sockaddr *)&opts->config.server.local.addr.un, 
            opts->config.server.local.len) != 0)
        return errno;

    return KYLIN_ERROR_OK;
}

ksock_conn_t *unix_accept(ksock_t *guard)
{
    kfd_t              fd;
    socklen_t          len;
    struct sockaddr_un client;

    ksock_conn_t *conn      = NULL;
    ksock_opts_t *opts      = NULL;
    ksock_unix_t *sock_unix = NULL;

    opts = kylin_socket_get_opts(guard);
    if(!opts || 
            opts->type != SOCK_STREAM ||
            !opts->config.server.local.len)
        return NULL;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix || sock_unix->fd == -1)
        return NULL;

    fd = accept(sock_unix->fd, (struct sockaddr *)&client, &len);
    if(fd == -1)
        return NULL;

#ifdef SO_NOSIGPIPE
    int set = 1;
    setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif

    conn = kylin_malloc(sizeof(ksock_conn_t));
    if(!conn) {
        close(fd);
        return NULL;
    }

    conn->fd         = fd;
    conn->sock       = guard;
    conn->client.len = len;
    memcpy(&conn->client.addr.un, &client, sizeof(struct sockaddr_un));
    memcpy(&conn->server, &opts->config.server.local, sizeof(ksock_addr_t));

    if(kylin_list_insert_head(sock_unix->conns, conn) != KYLIN_ERROR_OK) {
        close(conn->fd);
        kylin_free(conn);
        return NULL;
    }

    return conn;
}

ssize_t unix_recv(ksock_t *guard, kfd_t fd, void *buf, size_t len)
{
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_UNIX &&
            type != KYLIN_SOCK_CLIENT_UNIX)
        return 0;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return 0;

    if(opts->type == SOCK_STREAM)
        return recv(fd, buf, len, 0);
    else {
        if(type == KYLIN_SOCK_SERVER_UNIX) {
            return recvfrom(fd, buf, len, 0, 
                    (struct sockaddr *)&opts->config.client.remote.addr.un, 
                    &opts->config.client.remote.len);
        }
        else {
            return recvfrom(fd, buf, len, 0, 
                    (struct sockaddr *)&opts->config.server.local.addr.un, 
                    &opts->config.server.local.len);
        }
    }

    return 0;
}

ssize_t unix_send(ksock_t *guard, kfd_t fd, const void *buf, size_t len)
{
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_UNIX &&
            type != KYLIN_SOCK_CLIENT_UNIX)
        return 0;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return 0;

    if(opts->type == SOCK_STREAM)
        return send(fd, buf, len, 0);
    else {
        if(type == KYLIN_SOCK_SERVER_UNIX) {
            return sendto(fd, buf, len, 0, 
                    (struct sockaddr *)&opts->config.client.remote.addr.un, 
                    opts->config.client.remote.len);
        }
        else {
            return sendto(fd, buf, len, 0, 
                    (struct sockaddr *)&opts->config.server.local.addr.un, 
                    opts->config.server.local.len);
        }
    }

    return 0;
}

uint32_t unix_conn_count(ksock_t *guard)
{
    ksock_unix_t *sock_unix = NULL;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix)
        return 0;

    return kylin_list_count(sock_unix->conns);
}

ksock_conn_t *unix_conn_get_first(ksock_t *guard)
{
    klist_node_t *node     = NULL;
    ksock_unix_t *sock_unix = NULL;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix || !sock_unix->conns)
        return NULL;

    node = kylin_list_first(sock_unix->conns);
    if(!node)
        return NULL;

    return (ksock_conn_t *)kylin_list_val(sock_unix->conns, node);
}

ksock_conn_t *unix_conn_get_next(ksock_t *guard, ksock_conn_t *conn)
{
    klist_node_t *node     = NULL;
    ksock_unix_t *sock_unix = NULL;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix || !sock_unix->conns)
        return NULL;

    node = kylin_list_find_raw(sock_unix->conns, conn);
    if(!node)
        return NULL;

    node = kylin_list_next(sock_unix->conns, node);
    if(!node)
        return NULL;

    return (ksock_conn_t *)kylin_list_val(sock_unix->conns, node);
}

void unix_conn_destroy(ksock_t *guard, ksock_conn_t *conn)
{
    klist_node_t *node = NULL;
    ksock_unix_t *sock_unix = NULL;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix || !sock_unix->conns)
        return;

    node = kylin_list_unlink(sock_unix->conns, conn);
    if(!node)
        return;

    close(conn->fd);
    kylin_free(conn);
}

kfd_t unix_get_sockfd(ksock_t *guard)
{
    ksock_unix_t *sock_unix = NULL;

    sock_unix = (ksock_unix_t *)kylin_socket_get_priv(guard);
    if(!sock_unix)
        return -1;

    return sock_unix->fd;
}

static int __unix_connection_match(const void *val, const void *key)
{
    const ksock_conn_t *conn1 = val;
    const ksock_conn_t *conn2 = key;

    if(conn1->fd > conn2->fd)
        return 1;
    if(conn1->fd < conn2->fd)
        return -1;
    return 0;
}

kerr_t unix_init(void)
{
    return KYLIN_ERROR_OK;
}

void unix_fini(void)
{
    return;
}

