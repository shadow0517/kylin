#ifndef _KYLIN_CORE_EVENT_KQUEUE_H_
#define _KYLIN_CORE_EVENT_KQUEUE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

kerr_t kqueue_add(kevent_t *, kfd_t, kevent_flag_t);
kerr_t kqueue_del(kevent_t *, kfd_t, kevent_flag_t);
kerr_t kqueue_mod(kevent_t *, kfd_t, kevent_flag_t);

kerr_t kqueue_process(kevent_t *, uint64_t timeout);

kfd_t  kqueue_create(void);
void   kqueue_destroy(kfd_t);

kerr_t kqueue_init(void);
void   kqueue_fini(void);
#endif /*_KYLIN_CORE_EVENT_KQUEUE_H_*/
