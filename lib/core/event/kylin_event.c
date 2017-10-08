#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>

#include <kylin/include/math/kylin_list.h>
#include <kylin/include/math/kylin_rb.h>

typedef struct {
    kevent_type_t    type;
    kevent_actions_t actions;
    uint32_t         ref_cnt;
} kevent_mod_t;

struct kylin_event {
    kevent_type_t  type;
    kevent_opts_t  opts;
    kfd_t          efd;     /*event fd. when we call epoll or kqueue, it is*/
    kset_t        *events;  /*events set*/
};

static kevent_mod_t emod[KEVENT_TYPE_MAX];
static klist_t      *elist = NULL;

kevent_t *kylin_event_create(const kevent_type_t type, const kevent_opts_t *opts)
{

}

void kylin_event_destroy(kevent_t *event)
{

}

kerr_t kylin_event_add(kevent_t *event, kfd_t fd)
{

}

kerr_t kylin_event_del(kevent_t *event, kfd_t fd)
{

}

kerr_t kylin_event_process(kevent *event)
{

}

kerr_t kylin_event_register(const kevent_type_t type, const kevent_actions_t *actions)
{

}

void kylin_event_unregister(const kevent_type_t type)
{

}

kerr_t kylin_event_init(void)
{

}

void kylin_event_fini(void)
{

}

