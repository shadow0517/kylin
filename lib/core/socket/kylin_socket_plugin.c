#include <kylin/include/kylin.h>
#include <kylin/include/kylin_socket.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/socket/kylin_socket_plugin.h>
#include <kylin/lib/core/socket/plugin/kylin_socket_tcp.h>
#include <kylin/lib/core/socket/plugin/kylin_socket_udp.h>
#include <kylin/lib/core/socket/plugin/kylin_socket_unix.h>

#define KYLIN_SOCK_TYPE_DEF_CNT 3
#define KYLIN_SOCK_TYPE_MAX_CNT KYLIN_SOCK_SERVER_MAX

ksock_plugin_t splugin[KYLIN_SOCK_MAX] = {
    {
        .ref  = 0,
        .type = KYLIN_SOCK_SERVER_TCP,
        .reg  = {
            .create  = tcp_create,
            .destroy = tcp_destroy,
            .connect = NULL,
            .accept  = tcp_accept,
            .recv    = tcp_recv,
            .send    = tcp_send,
            .init    = tcp_init,
            .fini    = tcp_fini
        }
    },
    {
        .ref  = 0,
        .type = KYLIN_SOCK_SERVER_UDP,
        .reg  = {
            .create  = udp_create,
            .destroy = udp_destroy,
            .connect = NULL,
            .accept  = udp_accept,
            .recv    = udp_recv,
            .send    = udp_send,
            .init    = udp_init,
            .fini    = udp_fini
        }
    },
    {
        .ref  = 0,
        .type = KYLIN_SOCK_SERVER_UNIX,
        .reg  = {
            .create  = unix_create,
            .destroy = unix_destroy,
            .connect = NULL,
            .accept  = unix_accept,
            .recv    = unix_recv,
            .send    = unix_send,
            .init    = unix_init,
            .fini    = unix_fini
        }
    },
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {.type = KYLIN_SOCK_MAX},
    {
        .ref  = 0,
        .type = KYLIN_SOCK_CLIENT_TCP,
        .reg  = {
            .create  = tcp_create,
            .destroy = tcp_destroy,
            .connect = NULL,
            .accept  = tcp_accept,
            .recv    = tcp_recv,
            .send    = tcp_send,
            .init    = tcp_init,
            .fini    = tcp_fini
        }
    },
    {
        .ref  = 0,
        .type = KYLIN_SOCK_CLIENT_UDP,
        .reg  = {
            .create  = udp_create,
            .destroy = udp_destroy,
            .connect = NULL,
            .accept  = udp_accept,
            .recv    = udp_recv,
            .send    = udp_send,
            .init    = udp_init,
            .fini    = udp_fini
        }
    },
    {
        .ref  = 0,
        .type = KYLIN_SOCK_CLIENT_UNIX,
        .reg  = {
            .create  = unix_create,
            .destroy = unix_destroy,
            .connect = NULL,
            .accept  = unix_accept,
            .recv    = unix_recv,
            .send    = unix_send,
            .init    = unix_init,
            .fini    = unix_fini
        }
    } 
}; 

int kylin_socket_register(ksock_type_t type, ksock_reg_t *reg)
{
    kerr_t ret = KYLIN_ERROR_OK;

    if(splugin[type].type != KYLIN_SOCK_MAX)
        return KYLIN_ERROR_EXIST;

    if(!reg->create ||
            !reg->destroy)
        return KYLIN_ERROR_INVAL;

    if(reg->init) {
        ret = reg->init();
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    splugin[type].type = type;
    splugin[type].ref  = 0;
    memcpy(&splugin[type].reg, reg, sizeof(ksock_reg_t));

    return KYLIN_ERROR_OK;
}

void kylin_socket_unregister(ksock_type_t type)
{
    if(splugin[type].type == KYLIN_SOCK_MAX)
        return KYLIN_ERROR_NOENT;

    if(splugin[type].ref != 0)
        return KYLIN_ERROR_BUSY;

    if(splugin[type].reg.fini)
        eplugin[type].reg.fini();

    memset(&splugin[type], 0, sizeof(ksock_plugin_t));
    splugin[type].type = KYLIN_SOCK_MAX;

    return KYLIN_ERROR_OK;
}

kerr_t kylin_socket_plugin_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    for(int i = 0; i < KYLIN_SOCK_TYPE_DEF_CNT; i++) {
        if(splugin[i].type != KYLIN_SOCK_MAX &&
                splugin[i].reg.init) {
            ret = splugin[i].reg.init();
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }

        if(splugin[i + KYLIN_SOCK_TYPE_MAX_CNT].type != KYLIN_SOCK_MAX &&
                splugin[i + KYLIN_SOCK_TYPE_MAX_CNT].reg.init) {
            ret = splugin[i + KYLIN_SOCK_TYPE_MAX_CNT].reg.init();
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }

    for(int i = KEVENT_TYPE_DEF_CNT; i < KYLIN_SOCK_TYPE_MAX_CNT; i++) {
        splugin[i].type = KEVENT_TYPE_MAX;
        splugin[i + KYLIN_SOCK_TYPE_MAX_CNT].type = KEVENT_TYPE_MAX;
    }

    return KYLIN_ERROR_OK;
}

void kylin_socket_plugin_fini(void)
{
    for(int i = 0; i < KEVENT_TYPE_MAX; i++) {
        if(eplugin[i].type != KEVENT_TYPE_MAX &&
                eplugin[i].reg.fini)
            eplugin[i].reg.fini();
    }
}

