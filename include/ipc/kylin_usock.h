#ifndef _KYLIN_IPC_UNIXSOCK_H_
#define _KYLIN_IPC_UNIXSOCK_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

#define KYLIN_USOCK_PDU_PAYLOAD_MAX (65535)

struct kylin_usock_server;
struct kylin_usock_client;
typedef struct kylin_usock_server kusock_server_t;
typedef struct kylin_usock_client kusock_client_t;

typedef int (*kusock_init_t)(kusock_server_t *);
typedef int (*kusock_fini_t)(kusock_server_t *);
typedef int (*kusock_proc_t)(kusock_server_t *, unsigned long, void *);

extern kusock_server_t *kylin_usock_server_create(const char *, int mode,
        kusock_init_t init,
        kusock_fini_t fini,
        kusock_proc_t proc) 
    __kylin_nonnull((1, 5));
extern void kylin_usock_server_destroy(kusock_server_t *) 
    __kylin_nonnull((1));

extern kusock_client_t *kylin_usock_client_open(const char *) 
    __kylin_nonnull((1));
extern void kylin_usock_client_close(kusock_client_t *) 
    __kylin_nonnull((1));

extern kerr_t kylin_usock_client_request(kusock_client_t *, unsigned long, void *) 
    __kylin_nonnull((1)); 

#endif /*_KYLIN_IPC_UNIXSOCK_H_*/
