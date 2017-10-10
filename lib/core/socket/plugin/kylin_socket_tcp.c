#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_tcp.h>

typedef struct {
    kfd_t fd;
}ksock_tcp_t;

void *tcp_create(ksock_t *guard)
{

}

void  tcp_destroy(void *priv)
{

}

kerr_t tcp_connect(ksock_t *guard)
{

}

kerr_t tcp_accept(ksock_t *guard)
{

}

ssize_t tcp_recv(ksock_t *guard, void *buf, size_t len)
{

}

ssize_t tcp_send(ksock_t *guard, const void *buf, size_t len)
{

}

kerr_t tcp_init(void)
{

}

void tcp_fini(void)
{

}

