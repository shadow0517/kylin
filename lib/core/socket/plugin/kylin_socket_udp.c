#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_udp.h>

typedef struct {
    kfd_t fd;
}ksock_udp_t;

void *udp_create(ksock_t *guard)
{

}

void udp_destroy(void *priv)
{

}

kerr_t udp_connect(ksock_t *guard)
{

}

ksock_conn_t *udp_accept(ksock_t *guard)
{

}

ssize_t udp_recv(ksock_t *guard, void *buf, size_t len)
{

}

ssize_t udp_send(ksock_t *guard, const void *buf, size_t len)
{

}

kerr_t udp_init(void)
{

}

void udp_fini(void)
{

}

