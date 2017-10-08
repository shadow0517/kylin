#ifndef _KYLIN_CORE_EVENT_SELECT_H_
#define _KYLIN_CORE_EVENT_SELECT_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/utils/kylin_error.h>

kerr_t select_add(kevent_t *, kfd_t, uint32_t flags);
kerr_t select_del(kevent_t *, kfd_t, uint32_t flags);

#endif /*_KYLIN_CORE_EVENT_SELECT_H_*/
