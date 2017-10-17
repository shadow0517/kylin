#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>

#include <kylin/lib/core/event/kylin_event_plugin.h>

#if defined(__linux__)
#include <kylin/lib/core/event/plugin/kylin_select.h>
#include <kylin/lib/core/event/plugin/kylin_poll.h>
#include <kylin/lib/core/event/plugin/kylin_epoll.h>
#elif defined(__FreeBSD__)
#include <kylin/lib/core/event/plugin/kylin_kqueue.h>
#endif /*__linux__*/

/*
 * 全局的事件驱动模块，初始化默认的事件驱动机制，
 * 剩余的初始化工作在init完成
 */
#define KEVENT_TYPE_DEF_CNT  4

kevent_plugin_t eplugin[KEVENT_TYPE_MAX] = {
#if defined(__linux__)
    {
        .type = KEVENT_TYPE_SELECT, 
        .ref  = 0,
        .reg  = {
            .add     = kselect_add,
            .del     = kselect_del,
            .mod     = kselect_mod,
            .proc    = kselect_proc,
            .create  = kselect_create,
            .destroy = kselect_destroy,
            .init    = kselect_init,
            .fini    = kselect_fini
        }
    },
    {
        .type = KEVENT_TYPE_POLL, 
        .ref  = 0,
        .reg  = {
            .add     = kpoll_add,
            .del     = kpoll_del,
            .mod     = kpoll_mod,
            .proc    = kpoll_proc,
            .create  = kpoll_create,
            .destroy = kpoll_destroy,
            .init    = kpoll_init,
            .fini    = kpoll_fini
        }
    },
    {
        .type = KEVENT_TYPE_EPOLL, 
        .ref  = 0,
        .reg  = {
            .add     = kepoll_add,
            .del     = kepoll_del,
            .mod     = kepoll_mod,
            .proc    = kepoll_proc,
            .create  = kepoll_create,
            .destroy = kepoll_destroy,
            .init    = kepoll_init,
            .fini    = kepoll_fini
        }
    },
    {
        .type = KEVENT_TYPE_MAX, 
    }
#elif defined(__FreeBSD__)
    {
        .type = KEVENT_TYPE_MAX, 
    },
    {
        .type = KEVENT_TYPE_MAX, 
    },
    {
        .type = KEVENT_TYPE_MAX, 
    },
    {
        .type = KEVENT_TYPE_KQUEUE, 
        .ref  = 0,
        .reg  = {
            .add     = kqueue_add,
            .del     = kqueue_del,
            .mod     = kqueue_mod,
            .proc    = kqueue_proc,
            .create  = kqueue_create,
            .destroy = kqueue_destroy,
            .init    = kqueue_init,
            .fini    = kqueue_fini
        }

    }
#endif /*__SYS_LINUX__*/
};

kerr_t kylin_event_register(kevent_type_t type, const kevent_reg_t *reg)
{
    kerr_t ret = KYLIN_ERROR_OK;

    if(eplugin[type].type != KEVENT_TYPE_MAX)
        return KYLIN_ERROR_EXIST;

    if(!reg->proc)
        return KYLIN_ERROR_INVAL;

    if(reg->init) {
        ret = reg->init();
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    eplugin[type].type = type;
    eplugin[type].ref  = 0;
    memcpy(&eplugin[type].reg, reg, sizeof(kevent_reg_t));

    return KYLIN_ERROR_OK;
}

kerr_t kylin_event_unregister(kevent_type_t type)
{
    if(eplugin[type].type == KEVENT_TYPE_MAX)
        return KYLIN_ERROR_NOENT;

    if(eplugin[type].ref != 0)
        return KYLIN_ERROR_BUSY;

    if(eplugin[type].reg.fini)
        eplugin[type].reg.fini();

    memset(&eplugin[type], 0, sizeof(kevent_plugin_t));
    eplugin[type].type = KEVENT_TYPE_MAX;

    return KYLIN_ERROR_OK;
}

kerr_t kylin_event_plugin_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    for(int i = 0; i < KEVENT_TYPE_DEF_CNT; i++) {
        if(eplugin[i].type != KEVENT_TYPE_MAX &&
                eplugin[i].reg.init) {
            ret = eplugin[i].reg.init();
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }

    if(KEVENT_TYPE_MAX > KEVENT_TYPE_DEF_CNT) {
        for(int i = KEVENT_TYPE_DEF_CNT; i < KEVENT_TYPE_MAX; i++)
            eplugin[i].type = KEVENT_TYPE_MAX;
    }

    return KYLIN_ERROR_OK;
}

void kylin_event_plugin_fini(void)
{
    for(int i = 0; i < KEVENT_TYPE_MAX; i++) {
        if(eplugin[i].type != KEVENT_TYPE_MAX &&
                eplugin[i].reg.fini)
            eplugin[i].reg.fini();
    }
}

