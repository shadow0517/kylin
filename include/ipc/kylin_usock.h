#ifndef _KYLIN_IPC_UNIXSOCK_H_
#define _KYLIN_IPC_UNIXSOCK_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

struct kylin_usock_server;
struct kylin_usock_client;
typedef struct kylin_usock_server kusock_server_t;
typedef struct kylin_usock_client kusock_client_t;

typedef int (*kylin_usock_init_f)(kusock_server_t *);
typedef int (*kylin_usock_fini_f)(kusock_server_t *);
typedef int (*kylin_usock_proc_f)(kusock_server_t *, unsigned long, void *);

extern kusock_server_t *kylin_usock_server_create(const char *, int mode,
        kylin_usock_init_f init,
        kylin_usock_fini_f fini,
        kylin_usock_proc_f proc);
extern void kylin_usock_server_destroy(kusock_server_t *);

extern kusock_client_t *kylin_usock_client_open(const char *);
extern void kylin_usock_client_close(kusock_client_t *);

extern kerr_t kylin_usock_client_request(kusock_client_t *, unsigned long, void *); 

#endif /*_KYLIN_IPC_UNIXSOCK_H_*/
