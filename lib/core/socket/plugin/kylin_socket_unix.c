#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/plugin/kylin_socket_unix.h>

typedef struct {
    kfd_t fd;
}ksock_unix_t;

void *unix_create(ksock_t *guard)
{

}

void unix_destroy(void *priv)
{

}

kerr_t unix_connect(ksock_t *guard)
{

}

ksock_conn_t *unix_accept(ksock_t *guard)
{

}

ssize_t unix_recv(ksock_t *guard, void *buf, size_t len)
{

}

ssize_t unix_send(ksock_t *guard, const void *buf, size_t len)
{

}

kerr_t unix_init(void)
{

}

void unix_fini(void)
{

}

