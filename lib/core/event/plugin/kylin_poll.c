#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>

#include <kylin/lib/core/event/plugin/kylin_poll.h>
typedef struct {
    uint32_t pending;
}kevent_poll_t;

kerr_t kpoll_add(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    return KYLIN_ERROR_OK;
}

kerr_t kpoll_del(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    return KYLIN_ERROR_OK;
}

kerr_t kpoll_mod(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    return KYLIN_ERROR_OK;
}

kerr_t kpoll_proc(kevent_t *guard, uint64_t timeout)
{
    int             index = 0;
    int             nevents;
    uint32_t        ecnt;
    kevent_opts_t  *eopts = NULL;
    kevent_event_t *event = NULL;
    struct pollfd  *fds   = NULL;

    eopts = (kevent_opts_t *)kylin_event_get_opts(guard);
    ecnt  = kylin_event_event_count(guard);
    if(!eopts || ecnt == 0)
        return KYLIN_ERROR_NOENT;

    fds = malloc(sizeof(struct pollfd) * ecnt);
    if(!fds)
        return KYLIN_ERROR_NOMEM;
    memset(fds, 0, sizeof(struct pollfd) * ecnt);

    KYLIN_EVENT_EVENT_FOREACH(guard, event) {
        fds[index].fd = event->efd;

        if(event->flags & KEVENT_FLAG_READ)
            fds[index].events |= POLLIN;

        if(event->flags & KEVENT_FLAG_WRITE)
            fds[index].events |= POLLOUT;

        index++;
    }

    nevents = poll(fds, ecnt, timeout);
    if(nevents) {
        for(int i = 0; i < ecnt; i++) {
            if(fds[i].revents & POLLIN){
                if(eopts->action.recv)
                    eopts->action.recv(fds[i].fd, eopts->priv);
            }

            if(fds[i].revents & POLLOUT){
                if(eopts->action.send)
                    eopts->action.send(fds[i].fd, eopts->priv);
            }

            if(fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)){
                if(eopts->action.error)
                    eopts->action.error(fds[i].fd, eopts->priv);
            }
        }
    }

    free(fds);

    return KYLIN_ERROR_OK;
}

void *kpoll_create(void)
{
    kevent_poll_t *event_poll = NULL;

    event_poll = malloc(sizeof(kevent_poll_t));
    if(!event_poll)
        return NULL;

    return event_poll;
}

void kpoll_destroy(void *priv)
{
    kevent_poll_t *event_poll = (kevent_poll_t *)priv;

    if(event_poll) {
        free(event_poll);
    }

    return;
}

kerr_t kpoll_init(void)
{
    return KYLIN_ERROR_OK;
}

void kpoll_fini(void)
{
    return;
}

