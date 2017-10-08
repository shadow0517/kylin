#ifndef _KYLIN_CORE_EVENT_POLL_H_
#define _KYLIN_CORE_EVENT_POLL_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

kerr_t poll_add(kevent_t *, kfd_t, kevent_flag_t);
kerr_t poll_del(kevent_t *, kfd_t, kevent_flag_t);
kerr_t poll_mod(kevent_t *, kfd_t, kevent_flag_t);

kerr_t poll_process(kevent_t *, uint64_t timeout);

kfd_t  poll_create(void);
void   poll_destroy(kfd_t);

kerr_t poll_init(void);
void   poll_fini(void);
#endif /*_KYLIN_CORE_EVENT_POLL_H_*/
