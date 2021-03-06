#ifndef _KYLIN_CORE_SOCKET_PLUGIN_UNIX_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_UNIX_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

extern void *unix_create(ksock_t *);
extern void  unix_destroy(void *);

extern kerr_t unix_connect(ksock_t *); /*client*/
extern ksock_conn_t *unix_accept(ksock_t *);  /*server*/

extern ssize_t unix_recv(ksock_t *, kfd_t, void *, size_t);
extern ssize_t unix_send(ksock_t *, kfd_t, const void *, size_t);

extern uint32_t unix_conn_count(ksock_t *);
extern ksock_conn_t *unix_conn_get_first(ksock_t *);
extern ksock_conn_t *unix_conn_get_next(ksock_t *, ksock_conn_t *);
extern void unix_conn_destroy(ksock_t *, ksock_conn_t *);

extern kfd_t unix_get_sockfd(ksock_t *);

extern kerr_t unix_init(void);
extern void   unix_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_UNIX_H_*/
