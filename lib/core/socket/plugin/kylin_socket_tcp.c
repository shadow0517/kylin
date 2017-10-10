#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_tcp.h>

typedef struct {
    kfd_t fd;
}ksock_tcp_t;

void *tcp_create(ksock_t *guard)
{
    ksock_tcp_t  *sock_tcp = NULL;
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_TCP ||
            type != KYLIN_SOCK_CLIENT_TCP)
        return NULL;

    opts = kylin_socket_get_opts(guard);
    if(!opts || 
            !opts->config.server.addr.len ||
            !opts->config.server.backlog) 
        return NULL;

    sock_tcp = malloc(sizeof(ksock_tcp_t));
    if(!sock_tcp)
        return NULL;

    sock_tcp->fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_tcp->fd == -1) {
        free(sock_tcp);
        return NULL;
    }

    if(type == KYLIN_SOCK_SERVER_TCP) { /*server socket*/
        if(bind(sock_tcp->fd, 
                    (struct sockaddr *)&opts->config.server.addr.addr.in, 
                    opts->config.server.addr.len) != 0) {
            close(sock_tcp->fd);
            free(sock_tcp);
            return NULL;
        }

        if(listen(sock_tcp->fd, opts->config.server.backlog) != 0) {
            close(sock_tcp->fd);
            free(sock_tcp);
            return NULL;
        }
    }

    return sock_tcp;
}

void tcp_destroy(void *priv)
{
    ksock_tcp_t *sock_tcp = (ksock_tcp_t *)priv;

    if(sock_tcp) {
        close(sock_tcp->fd);
        free(sock_tcp);
    }

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
    if(!opts || !opts->config.client.remote.len) 
        return KYLIN_ERROR_NOENT;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || sock_tcp->fd == -1)
        return KYLIN_ERROR_IO;

    if(connect(sock_tcp->fd, 
            (struct sockaddr *)&opts->config.client.remote.addr.in, 
            opts->config.client.remote.len) != 0)
        return errno;

    return KYLIN_ERROR_OK;
}

ksock_conn_t *tcp_accept(ksock_t *guard)
{
    ksock_conn_t *conn = NULL;
    ksock_tcp_t  *sock_tcp = NULL;

    sock_tcp = (ksock_tcp_t *)kylin_socket_get_priv(guard);
    if(!sock_tcp || sock_tcp->fd == -1)
        return NULL;

    conn = malloc(sizeof(ksock_conn_t));
    if(!conn)
        return NULL;

    conn->fd = accept(sock_tcp->fd, 
            (struct sockaddr *)&conn->client.addr.in, 
            &conn->client.len);
    if(conn->fd == -1) {
        free(conn);
        return NULL;
    }

    return conn;
}

ssize_t tcp_recv(kfd_t fd, void *buf, size_t len)
{

}

ssize_t tcp_send(kfd_t fd, const void *buf, size_t len)
{

}

kerr_t tcp_init(void)
{

}

void tcp_fini(void)
{

}

