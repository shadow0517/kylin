#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>

#include <kylin/include/math/kylin_list.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/event/kylin_event.h>
#include <kylin/lib/core/event/kylin_event_plugin.h>

struct kylin_event {
    kevent_type_t  type;
    kevent_opts_t  opts;
    kfd_t          cfd;     /*event control fd. when we call epoll or kqueue, it is*/
    kset_t        *events;  /*events set, storage kevent_event_t*/
};

static klist_t      *elist = NULL;         /*list of kevent_t*/

static int __elist_match(const void *val, const void *key);
static int __events_compare(const void *v1, const void *v2);

kevent_t *kylin_event_create(const kevent_type_t type, const kevent_opts_t *opts)
{
    kevent_t *guard = NULL;

    if(eplugin[type].type == KEVENT_TYPE_MAX)
        return NULL;

    kset_opts_t events_opts = {
        .name      = "events_set",
        .is_order  = KYLIN_FALSE,
        .cap       = 4096,
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kevent_event_t),
        .compare   = __events_compare,
        .allocator = {
            .val_ctor   = malloc, /*值所在的内存由set模块分配*/
            .val_dtor   = NULL,
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    guard = malloc(sizeof(kevent_t));
    if(!guard)
        return NULL;

    guard->events = kylin_set_create(&event_opts);
    if(!guard->events) {
        free(guard);
        return NULL;
    }

    if(eplugin[type].reg.create) {
        guard->cfd = eplugin[type].reg.create(); 
        if(guard->cfd == -1) {
            kylin_set_destroy(guard->events);
            free(guard);
            return NULL;
        }
    }
    else
        guard->cfd = 0;

    if(!kylin_list_insert_head(elist, guard)) {
        if(eplugin[type].reg.destroy)
            eplugin[type].reg.destroy(guard->cfd);
        kylin_set_destroy(guard->events);
        free(guard);
        return NULL;
    }

    eplugin[type].ref++;
    guard->type = type;
    memcpy(&guard->opts, opts, sizeof(kevent_opts_t));
    
    return guard;
}

void kylin_event_destroy(kevent_t *guard)
{
    if(!kylin_list_unlink(elist, guard))
        return;

    if(eplugin[guard->type].reg.destroy)
        eplugin[guard->type].reg.destroy(guard->cfd);

    eplugin[guard->type].ref--;
    kylin_set_destroy(guard->events);
    free(guard);

    return;
}

kerr_t kylin_event_add(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_event_t event;

    memset(&event, 0, sizeof(kevent_event_t));
    event.efd = fd;
    event.flags = flags;

    ret = kylin_set_insert(guard->events, &event);
    if(ret != KYLIN_ERROR_OK)
        return ret;

    if(eplugin[guard->type].reg.add) {
        ret = eplugin[guard->type].reg.add(guard, fd, flags);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    return KYLIN_ERROR_OK;
}

kerr_t kylin_event_del(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_event_t cmp;
    kevent_event_t *event = NULL;

    memset(&cmp, 0, sizeof(kevent_event_t));
    cmp.efd = fd;
    cmp.flags = flags;

    event = (kevent_event_t *)kylin_set_find(guard->events, &cmp);
    if(!event)
        return KYLIN_ERROR_NOENT; 

    if(event->flags == flags) { /*delete the event*/
        if(eplugin[guard->type].reg.del) {
            ret = eplugin[guard->type].reg.del(guard, fd, flags);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
        ret = kylin_set_remove(guard->events, event);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }
    else { /*modify the event*/
        if(eplugin[guard->type].reg.mod) {
            ret = eplugin[guard->type].reg.mod(guard, fd, flags);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
        event->flags = flags;
    }

    return KYLIN_ERROR_OK;
}

kerr_t kylin_event_process(kevent *guard)
{
    kerr_t ret = KYLIN_ERROR_OK;

    if(eplugin[guard->type].reg.proc)
        ret = eplugin[guard->type].reg.proc(guard, 0);

    return ret;
}

kfd_t kylin_event_get_ctl_fd(kevent_t *guard)
{
    return guard->cfd;
}

kevent_opts_t *kylin_event_get_opts(kevent_t *guard)
{
    return &guard->opts;
}

kevent_event_t *kylin_event_event_get_first(kevent_t *guard)
{
    return (kevent_event_t *)kylin_set_first(guard->events);
}

kevent_event_t *kylin_event_event_get_next(kevent_t *guard, kevent_event_t *event)
{
    return (kevent_event_t *)kylin_set_next(guard->events, event);
}

static int __elist_match(const void *val, const void *key)
{
    const kevent_t *guard = val;
    const kevent_t *cmp   = key;

    if(guard->cfd > key->cfd)
        return 1;
    if(guard->cfd < key->cfd)
        return -1;
    return 0;
}

static int __events_compare(const void *v1, const void *v2)
{
    const kevent_event_t *e1 = v1;
    const kevent_event_t *e2 = v2;

    if(e1->efd > e2->efd)
        return 1;
    if(e1->efd < e2->efd)
        return -1;
    return 0;
}

kerr_t kylin_event_init(void)
{
    klist_opts_t elist_opts = {
        .name      = "event_list",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kevent_t),
        .match     = __elist_match,
        .allocator = {
            .val_ctor   = NULL, /*值所在的内存由调用者分配*/
            .val_dtor   = NULL,
            .node_ctor  = NULL,
            .node_dtor  = NULL, 
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    elist = kylin_list_create(&elist_opts);
    if(!elist)
        return KYLIN_ERROR_NOMEM;

    return kylin_event_plugin_init(void);
}

void kylin_event_fini(void)
{
    klist_node_t *node = NULL;

    if(elist) {
        KYLIN_LIST_FOREACH(elist, node)
            kylin_event_destroy((kevent_t *)kylin_list_val(elist, node));
        kylin_list_destroy(elist);
    }

    kylin_event_plugin_fini(void);
}

