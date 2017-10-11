#ifndef _KYLIN_CORE_SOCKET_PLUGIN_H_
#define _KYLIN_CORE_SOCKET_PLUGIN_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

typedef struct {
    uint32_t     ref;  /*reference count*/
    ksock_type_t type;
    ksock_reg_t  reg;
} ksock_plugin_t;

/*
 * 全局的套接字模块
 */
extern ksock_plugin_t splugin[KYLIN_SOCK_MAX]; 

extern kerr_t kylin_socket_plugin_init(void);
extern void kylin_socket_plugin_fini(void);

#endif /*_KYLIN_CORE_SOCKET_PLUGIN_H_*/
