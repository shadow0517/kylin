#ifndef _KYLIN_CORE_EVENT_KQUEUE_H_
#define _KYLIN_CORE_EVENT_KQUEUE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t kylin_kqueue_add(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kylin_kqueue_del(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t kylin_kqueue_mod(kevent_t *, kfd_t, kevent_flag_t);

extern kerr_t kylin_kqueue_proc(kevent_t *, uint64_t timeout);

extern void  *kylin_kqueue_create(void);
extern void   kylin_kqueue_destroy(void *);

extern kerr_t kylin_kqueue_init(void);
extern void   kylin_kqueue_fini(void);
#endif /*_KYLIN_CORE_EVENT_KQUEUE_H_*/
