#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/math/kylin_set.h>

#include <kylin/lib/core/event/plugin/kylin_select.h>

#define SELECT_FDSET_TYPE_NUM 3

typedef struct {
    fd_set   *rdsets;
    fd_set   *wtsets;
    fd_set   *exsets;
    kset_t   *fd_set[SELECT_FDSET_TYPE_NUM]; /*存放排好序的套接字*/
} kevent_select_t;

kerr_t kselect_add(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_select_t *event_select = NULL;

    event_select = (kevent_select_t *)kylin_event_get_priv_data(guard);
    if(!event_select)
        return KYLIN_ERROR_NOENT;

    if(flags & KEVENT_FLAG_READ) {
        FD_SET(fd, event_select->rdsets);
        ret = kylin_set_insert(event_select->fd_set[0], (void *)&fd);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    if(flags & KEVENT_FLAG_WRITE) {
        FD_SET(fd, event_select->wtsets);
        ret = kylin_set_insert(event_select->fd_set[1], (void *)&fd);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    if(flags & KEVENT_FLAG_ERROR) {
        FD_SET(fd, event_select->exsets);
        ret = kylin_set_insert(event_select->fd_set[2], (void *)&fd);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    return ret;
}

kerr_t kselect_del(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_select_t *event_select = NULL;

    event_select = (kevent_select_t *)kylin_event_get_priv_data(guard);
    if(!event_select)
        return KYLIN_ERROR_NOENT;

    if(flags & KEVENT_FLAG_READ) {
        FD_CLR(fd, event_select->rdsets);
        ret = kylin_set_remove(event_select->fd_set[0], (void *)&fd);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    if(flags & KEVENT_FLAG_WRITE) {
        FD_CLR(fd, event_select->wtsets);
        ret = kylin_set_remove(event_select->fd_set[1], (void *)&fd);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    if(flags & KEVENT_FLAG_ERROR) {
        FD_CLR(fd, event_select->exsets);
        ret = kylin_set_remove(event_select->fd_set[2], (void *)&fd);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    return ret;
}

kerr_t kselect_mod(kevent_t *guard, kfd_t fd, kevent_flag_t flags)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kevent_select_t *event_select = NULL;

    event_select = (kevent_select_t *)kylin_event_get_priv_data(guard);
    if(!event_select)
        return KYLIN_ERROR_NOENT;

    if(~flags & KEVENT_FLAG_READ) { /*当前标记中没有读事件*/
        if(FD_ISSET(fd, event_select->rdsets)) { /*检测到读事件，删除之*/
            FD_CLR(fd, event_select->rdsets);
            ret = kylin_set_remove(event_select->fd_set[0], (void *)&fd);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }
    else {
        if(!FD_ISSET(fd, event_select->rdsets)) { /*没有检测到读事件，添加之*/
            FD_SET(fd, event_select->rdsets);
            ret = kylin_set_insert(event_select->fd_set[0], (void *)&fd);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }

    if(~flags & KEVENT_FLAG_WRITE) { /*当前标记中没有写事件*/
        if(FD_ISSET(fd, event_select->wtsets)) { /*检测到写事件，删除之*/
            FD_CLR(fd, event_select->wtsets);
            ret = kylin_set_remove(event_select->fd_set[1], (void *)&fd);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }
    else {
        if(!FD_ISSET(fd, event_select->wtsets)) { /*没有检测到写事件，添加之*/
            FD_SET(fd, event_select->wtsets);
            ret = kylin_set_insert(event_select->fd_set[1], (void *)&fd);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }

    if(~flags & KEVENT_FLAG_ERROR) { /*当前标记中没有异常事件*/
        if(FD_ISSET(fd, event_select->exsets)) { /*检测到异常事件，删除之*/
            FD_CLR(fd, event_select->exsets);
            ret = kylin_set_remove(event_select->fd_set[2], (void *)&fd);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }
    else {
        if(!FD_ISSET(fd, event_select->exsets)) { /*没有检测到异常事件，添加之*/
            FD_SET(fd, event_select->exsets);
            ret = kylin_set_insert(event_select->fd_set[2], (void *)&fd);
            if(ret != KYLIN_ERROR_OK)
                return ret;
        }
    }

    return ret;
}

kerr_t kselect_proc(kevent_t *guard, uint64_t timeout)
{
    int            max_fd;
    struct timeval tv;
    void            *val          = NULL;
    kevent_opts_t   *eopts        = NULL;
    kevent_select_t *event_select = NULL;

    eopts        = (kevent_opts_t *)kylin_event_get_opts(guard);
    event_select = (kevent_select_t *)kylin_event_get_priv_data(guard);
    if(!eopts && !event_select)
        return KYLIN_ERROR_NOENT;

    max_fd = KYLIN_MAX(KYLIN_MAX(*(kfd_t *)kylin_set_last(event_select->fd_set[0]),
            *(kfd_t *)kylin_set_last(event_select->fd_set[1])), 
            *(kfd_t *)kylin_set_last(event_select->fd_set[2]));

    tv.tv_usec = 0;
    tv.tv_sec  = timeout;

    if(select(max_fd + 1, 
                event_select->rdsets, 
                event_select->wtsets, 
                event_select->exsets, &tv)) {

        KYLIN_SET_FOREACH(event_select->fd_set[0], val) {
            if(FD_ISSET(*(kfd_t *)val, event_select->rdsets) && eopts->action.recv)
                eopts->action.recv(*(kfd_t *)val, eopts->priv);
        }

        KYLIN_SET_FOREACH(event_select->fd_set[1], val) {
            if(FD_ISSET(*(kfd_t *)val, event_select->wtsets) && eopts->action.send)
                eopts->action.send(*(kfd_t *)val, eopts->priv);
        }

        KYLIN_SET_FOREACH(event_select->fd_set[2], val) {
            if(FD_ISSET(*(kfd_t *)val, event_select->exsets) && eopts->action.error)
                eopts->action.error(*(kfd_t *)val, eopts->priv);
        }
    }

    KYLIN_SET_FOREACH(event_select->fd_set[0], val) 
        FD_SET(*(kfd_t *)val, event_select->rdsets);

    KYLIN_SET_FOREACH(event_select->fd_set[1], val) 
        FD_SET(*(kfd_t *)val, event_select->wtsets);

    KYLIN_SET_FOREACH(event_select->fd_set[2], val) 
        FD_SET(*(kfd_t *)val, event_select->exsets);

    return KYLIN_ERROR_OK;
}

static int __fd_compare(const void *v1, const void *v2)
{
    const kfd_t *fd1 = v1;
    const kfd_t *fd2 = v2;

    if(*fd1 > *fd2)
        return 1;
    if(*fd1 < *fd2)
        return -1;
    return 0;
}

void *kselect_create(void)
{
    kevent_select_t *event_select = NULL;
    kset_opts_t fdset_opts = {
        .name      = "event_select",
        .cap       = 64,
        .val_type  = KOBJ_INT32,
        .val_size  = 4,
        .compare   = __fd_compare,
        .allocator = KSET_OPTS_ALLOCATOR_VAL(kylin_malloc)
    };

    event_select = kylin_malloc(sizeof(kevent_select_t));
    if(!event_select)
        return NULL;

    for(int i = 0; i < SELECT_FDSET_TYPE_NUM; i++) {
        event_select->fd_set[i] = kylin_set_create(&fdset_opts);
        if(!event_select->fd_set[i]) {
            while(i--)
                kylin_set_destroy(event_select->fd_set[i]);
            kylin_free(event_select);
            return NULL;
        }
    }

    FD_ZERO(event_select->rdsets);
    FD_ZERO(event_select->wtsets);
    FD_ZERO(event_select->exsets);

    return event_select;
}

void kselect_destroy(void *priv)
{
    kevent_select_t *event_select = (kevent_select_t *)priv;

    if(event_select) {
        for(int i = SELECT_FDSET_TYPE_NUM - 1; i >= 0; i--) 
            kylin_set_destroy(event_select->fd_set[i]);

        kylin_free(event_select);
    }

    return;
}

kerr_t kselect_init(void)
{
    return KYLIN_ERROR_OK;
}

void kselect_fini(void)
{
    return;
}

