#ifndef _KYLIN_CORE_EVENT_SELECT_H_
#define _KYLIN_CORE_EVENT_SELECT_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

kerr_t select_add(kevent_t *, kfd_t, kevent_flag_t);
kerr_t select_del(kevent_t *, kfd_t, kevent_flag_t);
kerr_t select_mod(kevent_t *, kfd_t, kevent_flag_t);

kerr_t select_process(kevent_t *, uint64_t timeout);

kfd_t  select_create(void);
void   select_destroy(kfd_t);

kerr_t select_init(void);
void   select_fini(void);
#endif /*_KYLIN_CORE_EVENT_SELECT_H_*/
