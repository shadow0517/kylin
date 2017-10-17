#ifndef _KYLIN_CORE_EVENT_EPOLL_H_
#define _KYLIN_CORE_EVENT_EPOLL_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t kepoll_add(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kepoll_del(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kepoll_mod(kevent_t *, kfd_t, kevent_flag_t);

extern kerr_t kepoll_proc(kevent_t *, uint64_t timeout);

extern void  *kepoll_create(void);
extern void   kepoll_destroy(void *);

extern kerr_t kepoll_init(void);
extern void   kepoll_fini(void);
#endif /*_KYLIN_CORE_EVENT_EPOLL_H_*/
