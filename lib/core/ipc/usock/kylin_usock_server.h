#ifndef _KYLIN_CORE_IPC_UNIXSOCK_SERVER_H_
#define _KYLIN_CORE_IPC_UNIXSOCK_SERVER_H_

#include <kylin/include/kylin.h>
#include <kylin/include/ipc/kylin_usock.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

#include <kylin/lib/core/ipc/usock/kylin_usock.h>

typedef struct {
    kfd_t            sock;
    int              pdu_pos;
    usock_pdu_t     *pdu;
    kusock_server_t *server;
} usock_accept_t;

struct kylin_usock_server {
    char             name[KYLIN_NAME_LENGTH];
    kfd_t            sock;
    klist_t         *acpt_q; /*accept 队列*/
    kspinlock_t      lock;

    kusock_init_t    init;
    kusock_fini_t    fini;
    kusock_proc_t    proc;
};

extern kerr_t server_init(void);
extern void server_fini(void);

#endif /*_KYLIN_CORE_IPC_UNIXSOCK_SERVER_H_*/
