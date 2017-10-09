#ifndef _KYLIN_CORE_EVENT_SELECT_H_
#define _KYLIN_CORE_EVENT_SELECT_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t select_add(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t select_del(kevent_t *, kfd_t, kevent_flag_t);
extern kerr_t select_mod(kevent_t *, kfd_t, kevent_flag_t);

extern kerr_t select_proc(kevent_t *, uint64_t timeout);

extern void  *select_create(void);
extern void   select_destroy(void *);

extern kerr_t select_init(void);
extern void   select_fini(void);
#endif /*_KYLIN_CORE_EVENT_SELECT_H_*/
