#ifndef _KYLIN_CORE_SOCKET_PLUGIN_UDP_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_UDP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

extern void *udp_create(ksock_t *);
extern void  udp_destroy(void *);

extern ssize_t udp_recv(ksock_t *, kfd_t, void *, size_t);
extern ssize_t udp_send(ksock_t *, kfd_t, const void *, size_t);

extern kfd_t udp_get_sockfd(ksock_t *);

extern kerr_t udp_init(void);
extern void   udp_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_UDP_H_*/
