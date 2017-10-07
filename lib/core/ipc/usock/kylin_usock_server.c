#include <kylin/include/kylin.h>
#include <kylin/include/ipc/kylin_usock.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/ipc/usock/kylin_usock_server.h>

/*存储所有已创建的服务端信息*/
static krb_t *serv_rb = NULL;
static pthread_t serv_tid;

static int __accept_node_match(void *val, void *key);

static int __server_socket_create(kfd_t *fd, const char *name, int mode)
{
    int len;
    struct sockaddr_un local;

    *fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if(*fd == -1)
        return errno;

    local.sun_family = AF_UNIX;
    sprintf(local.sun_path, "%s/%s", KYLIN_USOCK_LOCAL, name);
    len = sizeof(local.sun_family ) + strlen(local.sun_path) + 1;

    if(bind(*fd, (struct sockaddr *)&local, len) == -1)
        return errno;

#ifdef SO_NOSIGPIPE
    int set = 1;
    setsockopt(*fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif

#if defined(__linux__)
    fcntl(*fd, F_SETFL, fcntl(*fd, F_GETFL, 0) | O_NONBLOCK);
#elif defined(__FreeBSD__)
    int flags = fcntl(*fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(*fd, F_SETFL, flags);
#endif

    if(listen(*fd, 20) == -1)
        return errno;

    if(mode) {
        chmod(local.sun_path, mode);
    }
    else {
        chmod(local.sun_path, 
                S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
    }

    return 0;
}

kusock_server_t *kylin_usock_server_create(const char *name, int mode, 
        kusock_init_t init, kusock_fini_t fini, kusock_proc_t proc)
{
    kusock_server_t *serv = NULL;

    if(strlen(name) > KYLIN_NAME_LENGTH)
        return NULL;

    klist_opts_t serv_list_opts = {
        .name      = name,
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(usock_accept_t),
        .match     = __accept_node_match,
        .allocator = {
            .val_ctor   = NULL, /*值所在的内存由调用者分配*/
            .val_dtor   = NULL,
            .node_ctor  = NULL,
            .node_dtor  = NULL,
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    serv = malloc(sizeof(kusock_server_t));
    if(!serv)
        return NULL;

    memset(serv, 0, sizeof(kusock_server_t));

    if(__server_socket_create(&serv->sock, name, mode) != 0) {
        if(serv->sock)
            close(serv->sock);
        free(serv);
        return NULL;
    }

    serv->acpt_q = kylin_list_create(&serv_list_opts); 
    if(!serv->acpt_q) {
        close(serv->sock);
        free(serv);
        return NULL;
    }

    memcpy(serv->name, name, strlen(name));
    kylin_spinlock_init(&serv->lock);
    serv->init = init;
    serv->fini = fini;
    serv->proc = proc;

    if(!kylin_rb_insert(serv_rb, serv)) {
        kylin_list_destroy(serv->acpt_q);
        close(serv->sock);
        free(serv);
        return NULL;
    }

    if(serv->init)
        serv->init(serv);

    return serv;
}

void kylin_usock_server_destroy(kusock_server_t *serv)
{
    kylin_list_destroy(serv->acpt_q);
    close(serv->sock);
    kylin_rb_remove(serv_rb, kylin_rb_find(serv_rb, serv));
}

static int __serv_compare(const void *c1, const void *c2)
{
    const kusock_server_t *comp1 = c1;
    const kusock_server_t *comp2 = c2;

    if(comp1->sock > comp2->sock)
        return 1;
    if(comp1->sock < comp2->sock)
        return -1;
    return 0;
}

static int __accept_node_match(void *val, void *key)
{
    usock_accept_t *acpt = val;
    usock_accept_t *cmp  = key;

    if(acpt->sock > cmp->sock)
        return 1;
    if(acpt->sock < cmp->sock)
        return -1;
    return 0;
}

/*TODO 可以考虑采用事件驱动的方式处理套接字可读事件*/
static void *__server_process(void *arg __kylin_unused)
{
    return NULL;
}

kerr_t server_init(void)
{
    krb_opts_t usock_serv_opts = {
        .name      = "usock_server",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kusock_server_t),
        .compare   = __serv_compare,
        .allocator = {
            .val_ctor   = NULL, /*值所在的内存由调用者分配*/
            .val_dtor   = NULL,
            .node_ctor  = NULL,
            .node_dtor  = NULL,
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    serv_rb = kylin_rb_create(&usock_serv_opts);
    if(!serv_rb)
        return KYLIN_ERROR_NOMEM;

    return pthread_create(&serv_tid, NULL, __server_process, NULL);
}

void server_fini(void)
{
    if(serv_rb)
        kylin_rb_destroy(serv_rb);
}

