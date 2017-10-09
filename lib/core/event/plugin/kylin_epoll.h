#ifndef _KYLIN_CORE_EVENT_EPOLL_H_
#define _KYLIN_CORE_EVENT_EPOLL_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t kylin_epoll_add(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kylin_epoll_del(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kylin_epoll_mod(kevent_t *, kfd_t, kevent_flag_t);

extern kerr_t kylin_epoll_proc(kevent_t *, uint64_t timeout);

extern void  *kylin_epoll_create(void);
extern void   kylin_epoll_destroy(void *);

extern kerr_t kylin_epoll_init(void);
extern void   kylin_epoll_fini(void);
#endif /*_KYLIN_CORE_EVENT_EPOLL_H_*/
