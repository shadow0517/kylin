#ifndef _KYLIN_CORE_SOCKET_PLUGIN_UNIX_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_UNIX_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

void *unix_create(ksock_t *);
void  unix_destroy(void *);

kerr_t unix_connect(ksock_t *); /*client*/
kerr_t unix_accept(ksock_t *);  /*server*/

ssize_t unix_recv(ksock_t *, void *, size_t);
ssize_t unix_send(ksock_t *, const void *, size_t);

kerr_t unix_init(void);
void unix_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_UNIX_H_*/
