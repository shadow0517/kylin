#ifndef _KYLIN_CORE_SOCKET_PLUGIN_UDP_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_UDP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

extern void *udp_create(ksock_t *);
extern void  udp_destroy(void *);

extern kerr_t        udp_connect(ksock_t *); /*client*/
extern ksock_conn_t *udp_accept(ksock_t *);  /*server*/

extern ssize_t udp_recv(ksock_t *, void *, size_t);
extern ssize_t udp_send(ksock_t *, const void *, size_t);

extern kerr_t udp_init(void);
extern void   udp_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_UDP_H_*/
