#ifndef _KYLIN_EVENT_H_
#define _KYLIN_EVENT_H_

/*
 * 事件驱动模块
 */
struct kylin_event;
typedef struct kylin_event kevent_t;

typedef struct {
    void *priv;
    void (*kevent_recv_t)(kfd_t, void *);
    void (*kevent_send_t)(kfd_t, void *);
    void (*kevent_error_t)(kfd_t, void *);
    void (*kevent_timeout_t)(kfd_t, void *);
} kevent_action_t;

extern kevent_t *kylin_event_create(const kevent_action_t *);
extern void kylin_event_destroy(kevent_t *);

extern kerr_t kylin_event_add(kevent_t *, kfd_t);
extern kerr_t kylin_event_del(kevent_t *, kfd_t);

extern kerr_t kylin_event_process(kevent *);

#endif /*_KYLIN_EVENT_H_*/
