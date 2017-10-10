#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/kylin_socket_plugin.h>
#include <kylin/lib/core/socket/plugin/kylin_socket_tcp.h>
#include <kylin/lib/core/socket/plugin/kylin_socket_udp.h>
#include <kylin/lib/core/socket/plugin/kylin_socket_unix.h>

ksock_plugin_t splugin[KSOCK_TYPE_MAX] = {
    
    
    
}; 

int kylin_socket_register(ksock_type_t type, ksock_reg_t *reg)
{

}

void kylin_socket_unregister(ksock_type_t type)
{

}

kerr_t kylin_socket_plugin_init(void)
{

}

void kylin_socket_plugin_fini(void)
{

}

