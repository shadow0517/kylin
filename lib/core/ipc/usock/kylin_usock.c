#include <kylin/include/kylin.h>
#include <kylin/include/ipc/kylin_usock.h>

#include <kylin/lib/core/ipc/usock/kylin_usock.h>
#include <kylin/lib/core/ipc/usock/kylin_usock_client.h>
#include <kylin/lib/core/ipc/usock/kylin_usock_server.h>

kerr_t kylin_usock_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    ret = server_init();
    if(ret != KYLIN_ERROR_OK)
        return ret;

    ret = client_init();
    if(ret != KYLIN_ERROR_OK)
        return ret;

    return ret;
}

void kylin_usock_fini(void)
{
    client_fini();
    server_fini();
}

