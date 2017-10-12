#ifndef _KYLIN_CORE_IPC_UNIXSOCK_CLIENT_H_
#define _KYLIN_CORE_IPC_UNIXSOCK_CLIENT_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/ipc/kylin_usock.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

struct kylin_usock_client {
    kfd_t        fd;
    ksock_t     *sock;
    kspinlock_t  lock;
    char         name[KYLIN_NAME_LENGTH];
};

extern kerr_t client_init(void);
extern void client_fini(void);

#endif /*_KYLIN_CORE_IPC_UNIXSOCK_CLIENT_H_*/
