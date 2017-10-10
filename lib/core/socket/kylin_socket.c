#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>

struct kylin_socket {
    uint32_t     id;
    void        *priv; /*每个套接字的私有数据，由create生成，destroy销毁*/
    ksock_type_t type;
    ksock_opts_t opts
};

ksock_t *kylin_socket_create(ksock_type_t type, const ksock_opts_t *opts)
{

}

void kylin_socket_destroy(ksock_t *guard)
{

}

kerr_t kylin_socket_accept(ksock_t *guard)
{

}

kerr_t kylin_socket_connect(ksock_t *guard)
{

}

size_t kylin_socket_recv(ksock_t *guard, void *buf, size_t len)
{

}

size_t kylin_socket_send(ksock_t *guard, const void *buf, size_t len)
{

}

kfd_t kylin_socket_get_fd(ksock_t *guard)
{

}

ksock_opts_t *kylin_socket_get_opts(ksock_t *guard)
{

}

kerr_t kylin_socket_init(void)
{

}

void kylin_socket_fini(void)
{

}

