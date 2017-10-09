#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>

#include <kylin/lib/core/event/plugin/kylin_kqueue.h>

typedef struct {
    kfd_t         cfd;
    int           maxevents;
    struct kevent *events;
} kevent_kqueue_t;

kerr_t kqueue_add(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    int kev_count = 0;
    struct kevent kev[2];
    kevent_kqueue_t *event_kqueue = NULL;

    event_kqueue = (kevent_kqueue_t *)kylin_event_get_priv_data(guard);
    if(!event_kqueue)
        return KYLIN_ERROR_NOENT;

    memset(kev, 0, sizeof(kev) * 2);

    if(flags & KEVENT_FLAG_READ) {
        EV_SET(&kev[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, guard);
        kev_count = 1;
    }

    if(flags & KEVENT_FLAG_WRITE) {
        EV_SET(&kev[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, guard);
        kev_count = 2;
    }

    if(kevent(event_kqueue->cfd, kev, kev_count, NULL, 0, NULL ) != 0)
        return errno;
}

kerr_t kqueue_del(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    uint32_t filter = 0;
    struct kevent kev;
    kevent_kqueue_t *event_kqueue = NULL;

    event_kqueue = (kevent_kqueue_t *)kylin_event_get_priv_data(guard);
    if(!event_kqueue)
        return KYLIN_ERROR_NOENT;

    if(flags & KEVENT_FLAG_READ)
        filter |= EVFILT_READ;

    if(flags & KEVENT_FLAG_WRITE)
        filter |= EVFILT_WRITE;

    EV_SET(&kev, fd, filter, EV_DELETE, 0, 0, guard);

    if(kevent(event_kqueue->cfd, &kev, 1, NULL, 0, NULL) != 0)
        return errno;
}

kerr_t kqueue_mod(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_event_t *event = NULL;

    event = kylin_event_event_get_by_fd(guard, fd);
    if(!event)
        return KYLIN_ERROR_NOENT;

    ret = kqueue_del(guard, fd, event->flags);
    if(ret != KYLIN_ERROR_OK)
        return ret;

    return kqueue_add(guard, fd, flags);
}

kerr_t kqueue_proc(kevent_t *guard, uint64_t timeout)
{
    int             nevents;
    struct timespec ts;
    kevent_opts_t   *eopts        = NULL;
    kevent_kqueue_t *event_kqueue = NULL;

    eopts        = (kevent_opts_t *)kylin_event_get_opts(guard);
    event_kqueue = (kevent_kqueue_t *)kylin_event_get_priv_data(guard);
    if(!eopts || !event_kqueue)
        return KYLIN_ERROR_NOENT;

    ts.tv_sec = timeout;
    ts.tv_nsec = 0;

    nevents = kevent(event_kqueue->cfd, NULL, 0, 
            event_kqueue->events, event_kqueue->maxevents, &ts);
    for(int i = 0; i < nevents; i++) {
        if(event_kqueue->events[i].filter == EVFILT_READ) {
            if( event_kqueue->events[i].flags & EV_EOF || event_kqueue->events[i].flags & EV_ERROR) {
                if(eopts->action.error)
                    eopts->action.error(event_kqueue->events[i].ident, eopts->data);
            }
            else if(eopts->action.recv) {
                eopts->action.recv(event_kqueue->events[i].ident, eopts->data);
            }
        }
        else if(event_kqueue->events[i].filter == EVFILT_WRITE) {
            if( event_kqueue->events[i].flags & EV_EOF || event_kqueue->events[i].flags & EV_ERROR) {
                if(eopts->action.error)
                    eopts->action.error(event_kqueue->events[i].ident, eopts->data);
            }
            else if(eopts->action.send)
                eopts->action.send(event_kqueue->events[i].ident, eopts->data);
        }
        else if(event_kqueue->events[i].filter == EVFILT_TIMER) {
            if(eopts->action.timeout)
                eopts->action.timeout(event_kqueue->events[i].ident, eopts->data);
        }
    }

    return KYLIN_ERROR_OK;
}

void *kqueue_create(void)
{
    kevent_kqueue_t *event_kqueue = NULL;

    event_kqueue = malloc(sizeof(kevent_kqueue_t));
    if(!event_kqueue)
        return NULL;

    event_kqueue->maxevents = 64;

    event_kqueue->cfd = kqueue(); 
    if(event_kqueue->cfd == -1) {
        free(event_kqueue);
        return NULL;
    }

    event_kqueue->events = malloc(sizeof(struct kevent) * event_kqueue->maxevents);
    if(!event_kqueue) {
        close(event_kqueue->cfd);
        free(event_kqueue);
        return NULL;
    }

    return event_kqueue;
}

void kqueue_destroy(void *priv)
{
    kevent_kqueue_t *event_kqueue = (kevent_kqueue_t *)priv;
    close(event_kqueue->cfd);
    free(event_kqueue->events);
    free(event_kqueue);
}

kerr_t kqueue_init(void)
{
    return KYLIN_ERROR_OK;
}

void kqueue_fini(void)
{
    return;
}

