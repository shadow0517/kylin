#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_udp.h>

typedef struct {
    kfd_t fd;
}ksock_udp_t;

void *udp_create(ksock_t *guard)
{
    ksock_udp_t  *sock_udp = NULL;
    ksock_opts_t *opts = NULL;
    ksock_type_t  type;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_UDP &&
            type != KYLIN_SOCK_CLIENT_UDP)
        return NULL;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return NULL;

    sock_udp = malloc(sizeof(ksock_udp_t));
    if(!sock_udp)
        return NULL;

    sock_udp->fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock_udp->fd == -1) {
        free(sock_udp);
        return NULL;
    }

    if(type == KYLIN_SOCK_SERVER_UDP) { /*server socket*/
        if(bind(sock_udp->fd, 
                    (struct sockaddr *)&opts->config.server.local.addr.in, 
                    opts->config.server.local.len) != 0) {
            close(sock_udp->fd);
            free(sock_udp);
            return NULL;
        }
    }

    return sock_udp;
}

void udp_destroy(void *priv)
{
    ksock_udp_t *sock_udp = (ksock_udp_t *)priv;

    if(sock_udp) {
        if(sock_udp->fd)
            close(sock_udp->fd);
        free(sock_udp);
    }

    return;
}

ssize_t udp_recv(ksock_t *guard, kfd_t fd, void *buf, size_t len)
{
    ksock_type_t  type;
    ksock_opts_t *opts = NULL;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_UDP &&
            type != KYLIN_SOCK_CLIENT_UDP)
        return 0;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return 0;

    if(type == KYLIN_SOCK_SERVER_UDP) {
        return recvfrom(fd, buf, len, 0, 
                (struct sockaddr *)&opts->config.client.remote.addr.un, 
                &opts->config.client.remote.len);
    }
    else {
        return recvfrom(fd, buf, len, 0, 
                (struct sockaddr *)&opts->config.server.local.addr.un, 
                &opts->config.server.local.len);
    }

    return 0;
}

ssize_t udp_send(ksock_t *guard, kfd_t fd, const void *buf, size_t len)
{
    ksock_type_t  type;
    ksock_opts_t *opts = NULL;

    type = kylin_socket_get_type(guard);
    if(type != KYLIN_SOCK_SERVER_UDP &&
            type != KYLIN_SOCK_CLIENT_UDP)
        return 0;

    opts = kylin_socket_get_opts(guard);
    if(!opts) 
        return 0;

    if(type == KYLIN_SOCK_SERVER_UDP) {
        return sendto(fd, buf, len, 0, 
                (struct sockaddr *)&opts->config.client.remote.addr.un, 
                opts->config.client.remote.len);
    }
    else {
        return sendto(fd, buf, len, 0, 
                (struct sockaddr *)&opts->config.server.local.addr.un, 
                opts->config.server.local.len);
    }

    return 0;

}

kerr_t udp_init(void)
{
    return KYLIN_ERROR_OK;
}

void udp_fini(void)
{
    return;
}

