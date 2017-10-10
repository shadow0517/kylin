#ifndef _KYLIN_CORE_SOCKET_PLUGIN_UDP_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_UDP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

void *udp_create(ksock_t *);
void  udp_destroy(void *);

kerr_t udp_connect(ksock_t *); /*client*/
kerr_t udp_accept(ksock_t *);  /*server*/

ssize_t udp_recv(ksock_t *, void *, size_t);
ssize_t udp_send(ksock_t *, const void *, size_t);

kerr_t udp_init(void);
void udp_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_UDP_H_*/
