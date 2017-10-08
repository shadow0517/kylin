#ifndef _KYLIN_CORE_EVENT_EPOLL_H_
#define _KYLIN_CORE_EVENT_EPOLL_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

kerr_t epoll_add(kevent_t *, kfd_t, kevent_flag_t);
kerr_t epoll_del(kevent_t *, kfd_t, kevent_flag_t);
kerr_t epoll_mod(kevent_t *, kfd_t, kevent_flag_t);

kerr_t epoll_process(kevent_t *, uint64_t timeout);

kfd_t  epoll_create(void);
void   epoll_destroy(kfd_t);

kerr_t epoll_init(void);
void   epoll_fini(void);
#endif /*_KYLIN_CORE_EVENT_EPOLL_H_*/
