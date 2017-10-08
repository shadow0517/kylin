#ifndef _KYLIN_CORE_EVENT_PLUGIN_H_
#define _KYLIN_CORE_EVENT_PLUGIN_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

typedef struct {
    uint32_t      ref;  /*reference count*/
    kevent_type_t type;
    kevent_reg_t  reg;
} kevent_plugin_t;

/*
 * 全局的事件驱动模块
 */
extern kevent_plugin_t eplugin[KEVENT_TYPE_MAX]; 

extern kerr_t kylin_event_plugin_init(void);
extern void kylin_event_plugin_fini(void);

#endif /*_KYLIN_CORE_EVENT_PLUGIN_H_*/
