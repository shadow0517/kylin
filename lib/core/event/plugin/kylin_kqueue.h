#ifndef _KYLIN_CORE_EVENT_KQUEUE_H_
#define _KYLIN_CORE_EVENT_KQUEUE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t kqueue_add(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kqueue_del(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kqueue_mod(kevent_t *, kfd_t, kevent_flag_t);

extern kerr_t kqueue_proc(kevent_t *, uint64_t timeout);

extern void  *kqueue_create(void);
extern void   kqueue_destroy(void *);

extern kerr_t kqueue_init(void);
extern void   kqueue_fini(void);
#endif /*_KYLIN_CORE_EVENT_KQUEUE_H_*/
