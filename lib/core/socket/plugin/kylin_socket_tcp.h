#ifndef _KYLIN_CORE_SOCKET_PLUGIN_TCP_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_TCP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

void *tcp_create(ksock_t *);
void  tcp_destroy(void *);

kerr_t tcp_connect(ksock_t *); /*client*/
kerr_t tcp_accept(ksock_t *);  /*server*/

ssize_t tcp_recv(ksock_t *, void *, size_t);
ssize_t tcp_send(ksock_t *, const void *, size_t);

kerr_t tcp_init(void);
void tcp_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_TCP_H_*/
