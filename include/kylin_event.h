#ifndef _KYLIN_EVENT_H_
#define _KYLIN_EVENT_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

/*
 * 事件驱动模块
 * 系统默认提供select, poll, epoll和kqueue四种事件驱动机制，
 * 用户可以添加其他事件驱动机制的类型 
 */
typedef enum {
    KEVENT_TYPE_SELECT  = 0x00,
    KEVENT_TYPE_POLL    = 0x01,
    KEVENT_TYPE_EPOLL   = 0x02,
    KEVENT_TYPE_KQUEUE  = 0x03,
    KEVENT_TYPE_MAX     = 0x04
} kevent_type_t;

struct kylin_event;
typedef struct kylin_event kevent_t;

/*支持的事件类型*/
typedef enum {
    KEVENT_FLAG_NONE    = 0UL,
    KEVENT_FLAG_READ    = 1UL << 0,
    KEVENT_FLAG_WRITE   = 1UL << 1,
    KEVENT_FLAG_ERROR   = 1UL << 2,
} kevent_flag_t;

/*一个事件由事件描述符（fd）和事件类型（flags）组成*/
typedef struct {
    kfd_t         efd;
    kevent_flag_t flags;
} kevent_event_t;

/*
 * 使用事件驱动机制的接口
 */
typedef struct {
    void *priv;
    struct {
        void (*recv)(kfd_t, void *);
        void (*send)(kfd_t, void *);
        void (*error)(kfd_t, void *);
        void (*timeout)(kfd_t, void *);
    } action;
} kevent_opts_t;

extern kevent_t *kylin_event_create(const kevent_type_t, const kevent_opts_t *);
extern void kylin_event_destroy(kevent_t *);

extern kerr_t kylin_event_add(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kylin_event_del(kevent_t *, kfd_t, kevent_flag_t);

extern kerr_t kylin_event_process(kevent_t *);

extern void *kylin_event_get_priv_data(kevent_t *);
extern kevent_opts_t *kylin_event_get_opts(kevent_t *);
extern uint32_t kylin_event_event_count(kevent_t *);
extern kevent_event_t *kylin_event_event_get_first(kevent_t *);
extern kevent_event_t *kylin_event_event_get_next(kevent_t *, kevent_event_t *);
extern kevent_event_t *kylin_event_event_get_by_fd(kevent_t *, kfd_t);

#define KYLIN_EVENT_EVENT_FOREACH(guard, event)               \
    for(event = kylin_event_event_get_first(guard);           \
            event != NULL;                                    \
            event = kylin_event_event_get_next(guard, event))

/*
 * 注册事件驱动机制的接口
 */
typedef struct {
    kerr_t (*add)(kevent_t *, kfd_t, kevent_flag_t);
    kerr_t (*del)(kevent_t *, kfd_t, kevent_flag_t);
    kerr_t (*mod)(kevent_t *, kfd_t, kevent_flag_t);

    kerr_t (*proc)(kevent_t *, uint64_t timeout);

    void  *(*create)(void);
    void   (*destroy)(void *);

    kerr_t (*init)(void);
    void   (*fini)(void);
} kevent_reg_t;

extern kerr_t kylin_event_register(kevent_type_t, const kevent_reg_t *);
extern kerr_t kylin_event_unregister(kevent_type_t);

#endif /*_KYLIN_EVENT_H_*/
