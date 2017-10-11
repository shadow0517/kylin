#ifndef _KYLIN_CORE_SOCKET_PLUGIN_TCP_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_TCP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

extern void *tcp_create(ksock_t *);
extern void  tcp_destroy(void *);

extern kerr_t tcp_connect(ksock_t *); /*client*/
extern ksock_conn_t *tcp_accept(ksock_t *);  /*server*/

extern ssize_t tcp_recv(ksock_t *, kfd_t, void *, size_t);
extern ssize_t tcp_send(ksock_t *, kfd_t, const void *, size_t);

extern uint32_t tcp_conn_count(ksock_t *);
extern ksock_conn_t *tcp_conn_get_first(ksock_t *);
extern ksock_conn_t *tcp_conn_get_next(ksock_t *, ksock_conn_t *);
extern void tcp_conn_destroy(ksock_t *, ksock_conn_t *);

extern kerr_t tcp_init(void);
extern void tcp_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_TCP_H_*/
