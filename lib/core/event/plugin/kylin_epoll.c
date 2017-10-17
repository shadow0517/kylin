#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>

#include <kylin/lib/core/event/plugin/kylin_epoll.h>

typedef struct {
    kfd_t               cfd;
    int                 maxevents;
    struct epoll_event *events;
}kevent_epoll_t;

kerr_t kepoll_add(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    struct epoll_event ev = { 0 };
    kevent_epoll_t *event_epoll = NULL;

    event_epoll = (kevent_epoll_t *)kylin_event_get_priv_data(guard);
    if(!event_epoll)
        return KYLIN_ERROR_NOENT;

    if(flags & KEVENT_FLAG_READ) {
        ev.data.fd = fd;
        ev.events  = EPOLLIN | EPOLLRDHUP;

        ret = epoll_ctl(event_epoll->cfd, EPOLL_CTL_ADD, fd, &ev);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    if(flags & KEVENT_FLAG_WRITE) {
        ev.data.fd = fd;
        ev.events  = EPOLLOUT | EPOLLRDHUP;

        ret = epoll_ctl(event_epoll->cfd, EPOLL_CTL_ADD, fd, &ev);
        if(ret != KYLIN_ERROR_OK) 
            return ret;
    }

    return ret;
}

kerr_t kepoll_del(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    struct epoll_event ev = { 0 };
    kevent_epoll_t *event_epoll = NULL;

    event_epoll = (kevent_epoll_t *)kylin_event_get_priv_data(guard);
    if(!event_epoll)
        return KYLIN_ERROR_NOENT;

    if(flags & KEVENT_FLAG_READ) {
        ev.data.fd = fd;
        ev.events  = EPOLLIN;

        ret = epoll_ctl(event_epoll->cfd, EPOLL_CTL_DEL, fd, &ev);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    if(flags & KEVENT_FLAG_WRITE) {
        ev.data.fd = fd;
        ev.events  = EPOLLOUT;

        ret = epoll_ctl(event_epoll->cfd, EPOLL_CTL_DEL, fd, &ev);
        if(ret != KYLIN_ERROR_OK) 
            return ret;
    }

    return ret;
}

kerr_t kepoll_mod(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_event_t *event = NULL;

    event = kylin_event_event_get_by_fd(guard, fd);
    if(!event)
        return KYLIN_ERROR_NOENT;

    ret = kepoll_del(guard, fd, event->flags);
    if(ret != KYLIN_ERROR_OK)
        return ret;

    return kepoll_add(guard, fd, flags);
}

kerr_t kepoll_proc(kevent_t *guard, uint64_t timeout)
{
    int      nevents;
    kfd_t    fd;
    uint32_t flags;
    kevent_opts_t  *eopts       = NULL;
    kevent_epoll_t *event_epoll = NULL;

    eopts       = (kevent_opts_t *)kylin_event_get_opts(guard);
    event_epoll = (kevent_epoll_t *)kylin_event_get_priv_data(guard);
    if(!eopts || !event_epoll)
        return KYLIN_ERROR_NOENT;

    nevents = epoll_wait(event_epoll->cfd, event_epoll->events, event_epoll->maxevents, timeout);
    for(int i = 0; i < nevents; i++) {
        fd    = event_epoll->events[i].data.fd;
        flags = event_epoll->events[i].events;

		if (flags & (EPOLLHUP|EPOLLERR)) {
			if(eopts->action.error)
				eopts->action.error(fd, eopts->priv);
		} 
        else {
			if (flags & EPOLLIN) {
				if(eopts->action.recv)
					eopts->action.recv(fd, eopts->priv);
			}
			if (flags & EPOLLOUT) {
				if(eopts->action.send)
                	eopts->action.send(fd, eopts->priv);
			}
			if (flags & EPOLLRDHUP) {
				if(eopts->action.error)
					eopts->action.error(fd, eopts->priv);
			}
		}
    }

    return KYLIN_ERROR_OK;
}

void *kepoll_create(void)
{
    kevent_epoll_t *event_epoll = NULL;

    event_epoll = malloc(sizeof(kevent_epoll_t));
    if(!event_epoll)
        return NULL;

    event_epoll->maxevents = 64;

    /*The size field is ignored since 2.6.8*/
    event_epoll->cfd = epoll_create(event_epoll->maxevents); 
    if(event_epoll->cfd == -1) {
        free(event_epoll);
        return NULL;
    }

    event_epoll->events = malloc(sizeof(struct epoll_event) * event_epoll->maxevents);
    if(!event_epoll) {
        close(event_epoll->cfd);
        free(event_epoll);
        return NULL;
    }

    return event_epoll;
}

void kepoll_destroy(void *priv)
{
    kevent_epoll_t *event_epoll = (kevent_epoll_t *)priv;

    if(event_epoll) {
        close(event_epoll->cfd);
        free(event_epoll->events);
        free(event_epoll);
    }

    return;
}

kerr_t kepoll_init(void)
{
    return KYLIN_ERROR_OK;
}

void kepoll_fini(void)
{
    return;
}


