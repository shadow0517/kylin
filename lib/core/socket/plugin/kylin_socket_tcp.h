#ifndef _KYLIN_CORE_SOCKET_PLUGIN_TCP_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_TCP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

extern void *tcp_create(ksock_t *);
extern void  tcp_destroy(void *);

extern kerr_t tcp_connect(ksock_t *); /*client*/
extern ksock_conn_t *tcp_accept(ksock_t *);  /*server*/

ssize_t tcp_recv(kfd_t, void *, size_t);
ssize_t tcp_send(kfd_t, const void *, size_t);

kerr_t tcp_init(void);
void tcp_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_TCP_H_*/
