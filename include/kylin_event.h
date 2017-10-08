#ifndef _KYLIN_EVENT_H_
#define _KYLIN_EVENT_H_

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

/*
 * 使用事件驱动机制的接口
 */
typedef struct {
    void               *priv;
    struct {
        void (*recv)(kfd_t, void *);
        void (*send)(kfd_t, void *);
        void (*error)(kfd_t, void *);
        void (*timeout)(kfd_t, void *);
    } callback;
} kevent_opts_t;

extern kevent_t *kylin_event_create(const kevent_type_t, const kevent_opts_t *);
extern void kylin_event_destroy(kevent_t *);

extern kerr_t kylin_event_add(kevent_t *, kfd_t);
extern kerr_t kylin_event_del(kevent_t *, kfd_t);

extern kerr_t kylin_event_process(kevent *);

/*
 * 注册事件驱动机制的接口
 */
typedef struct {
    kerr_t (*add)(kevent_t *, kfd_t, uint32_t flags);
    kerr_t (*del)(kevent_t *, kfd_t, uint32_t flags);

    kerr_t (*enable)(kevent_t *, kfd_t, uint32_t flags);
    kerr_t (*disable)(kevent_t *, kfd_t, uint32_t flags);

    kerr_t (*add_conn)(ngx_connection_t *c);
    kerr_t (*del_conn)(ngx_connection_t *c, uint32_t flags);

    kerr_t (*process_changes)(ngx_cycle_t *cycle, uint32_t nowait);
    kerr_t (*process_events)(ngx_cycle_t *cycle, ngx_msec_t timer, uint32_t flags);

    kerr_t (*init)(void);
    void   (*fini)(void);
} kevent_actions_t;

extern kerr_t kylin_event_register(const kevent_type_t, const kevent_actions_t *);
extern void kylin_event_unregister(const kevent_type_t);

#endif /*_KYLIN_EVENT_H_*/
