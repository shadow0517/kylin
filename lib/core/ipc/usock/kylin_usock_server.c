#include <kylin/include/kylin.h>
#include <kylin/include/kylin_event.h>
#include <kylin/include/ipc/kylin_usock.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/ipc/usock/kylin_usock_server.h>

static pthread_t serv_tid;

/*存储所有已创建的服务端信息*/
static krb_t *serv_rb = NULL;

/*accept_event监听服务器socket，process_event监听已到达的连接*/
static kevent_t *accept_event = NULL;
static kevent_t *process_event = NULL;

/*accept_event和process_event携带的私有数据，起到查找加速的作用*/
static kusock_server_t *acpt_priv = NULL;
static usock_accept_t  *proc_priv = NULL; 

static int __accept_node_match(const void *val, const void *key);

kusock_server_t *kylin_usock_server_create(const char *name, int mode, 
        kusock_init_t init, kusock_fini_t fini, kusock_proc_t proc)
{
    kusock_server_t *serv = NULL;
    struct sockaddr_un local;

    if(strlen(name) > KYLIN_NAME_LENGTH)
        return NULL;

    local.sun_family = AF_UNIX;
    sprintf(local.sun_path, "%s/%s", KYLIN_USOCK_LOCAL, name);

    if(mode)
        chmod(local.sun_path, mode);
    else 
        chmod(local.sun_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | 
                S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);

    ksock_opts_t sock_opts = {
        .type   = SOCK_STREAM,
        .config = {
            .server = {
                .backlog = 20,
                .local   = {
                    .len  = sizeof(local.sun_family) + strlen(local.sun_path) + 1,
                    .addr = {
                        .un = local 
                    }
                }
            }
        }
    };

    klist_opts_t acpt_opts = {
        .name      = name,
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(usock_accept_t),
        .match     = __accept_node_match,
        .allocator = {
            .val_ctor   = malloc, /*值所在的内存由list分配*/
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

    serv->sock = kylin_socket_create(KYLIN_SOCK_SERVER_UNIX, &sock_opts);
    if(!serv->sock) {
        free(serv);
        return NULL;
    }

    serv->fd = kylin_socket_get_fd(serv->sock);
    if(serv->fd == -1) {
        kylin_socket_destroy(serv->sock);
        free(serv);
        return NULL;
    }

    if(kylin_event_add(accept_event, serv->fd, KEVENT_FLAG_READ) != KYLIN_ERROR_OK) {
        kylin_socket_destroy(serv->sock);
        free(serv);
        return NULL;
    }

    serv->acpt_q = kylin_list_create(&acpt_opts); 
    if(!serv->acpt_q) {
        kylin_socket_destroy(serv->sock);
        kylin_event_del(accept_event, serv->fd, KEVENT_FLAG_READ);
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
        kylin_socket_destroy(serv->sock);
        kylin_event_del(accept_event, serv->fd, KEVENT_FLAG_READ);
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
    kylin_socket_destroy(serv->sock);
    kylin_rb_remove(serv_rb, serv);
}

static int __serv_compare(const void *c1, const void *c2)
{
    const kusock_server_t *comp1 = c1;
    const kusock_server_t *comp2 = c2;

    if(comp1->fd > comp2->fd)
        return 1;
    if(comp1->fd < comp2->fd)
        return -1;
    return 0;
}

static int __accept_node_match(const void *val, const void *key)
{
    const usock_accept_t *acpt = val;
    const usock_accept_t *cmp  = key;

    if(acpt->sock > cmp->sock)
        return 1;
    if(acpt->sock < cmp->sock)
        return -1;
    return 0;
}

static void *__server_event_process(void *arg __kylin_unused)
{
    return NULL;
}

static void __server_accept(kfd_t fd, void *priv)
{
    kusock_server_t *serv = priv;
    ksock_conn_t    *conn = NULL;
    kusock_server_t cmp;
    usock_accept_t  acpt;

    if(serv->fd != fd) {
        memset(&cmp, 0, sizeof(kusock_server_t));
        cmp.fd = fd;

        serv = (kusock_server_t *)kylin_rb_find(serv_rb, &cmp);
        if(!serv)
            return;

        memcpy((kusock_server_t *)priv, serv, sizeof(kusock_server_t));
    }

    conn = kylin_socket_accept(serv->sock);
    if(!conn)
        return;

    if(kylin_event_add(process_event, conn->fd, 
                KEVENT_FLAG_READ | KEVENT_FLAG_WRITE | KEVENT_FLAG_WRITE) 
            != KYLIN_ERROR_OK) {
        goto error;
    }

    memset(&acpt, 0, sizeof(usock_accept_t));
    acpt.fd     = conn->fd;
    acpt.sock   = conn->sock;
    acpt.server = serv;
    acpt.pdu    = malloc(sizeof(usock_pdu_t) + KYLIN_USOCK_PDU_PAYLOAD_MAX);
    if(!acpt.pdu) {
        kylin_event_del(process_event, conn->fd, 
                KEVENT_FLAG_READ | KEVENT_FLAG_WRITE | KEVENT_FLAG_WRITE);
        goto error;
    }

    if(!kylin_list_insert_head(serv->acpt_q, &acpt)) {
        kylin_event_del(process_event, conn->fd, 
                KEVENT_FLAG_READ | KEVENT_FLAG_WRITE | KEVENT_FLAG_WRITE);
        goto error;
    }

error:
    kylin_socket_connection_destroy(serv->sock, conn);
    return;
}

static __kylin_inline usock_accept_t *__accept_node_find(kfd_t fd, void *priv)
{
    usock_accept_t  *acpt      = priv;
    kusock_server_t *serv      = NULL;
    krb_node_t      *serv_node = NULL;
    klist_node_t    *acpt_node = NULL;

    if(acpt->fd != fd) {
        KYLIN_RB_FOREACH(serv_rb, serv_node) {
            serv = kylin_rb_val(serv_rb, serv_node);
            KYLIN_LIST_FOREACH(serv->acpt_q, acpt_node) {
                acpt = kylin_list_val(serv->acpt_q, acpt_node);
                if(acpt->fd == fd) {
                    memcpy(priv, acpt, sizeof(usock_accept_t));
                    return acpt;
                }
            }
        }
    }

    if(acpt->fd != fd)
        return NULL;

    return acpt;
}

static void __server_process_error(kfd_t fd, void *priv)
{
    usock_accept_t  *acpt = NULL;

    acpt = __accept_node_find(fd, priv);
    if(!acpt)
        return;

    memset(acpt->pdu, 0, sizeof(usock_pdu_t) + KYLIN_USOCK_PDU_PAYLOAD_MAX);
}

static void __server_process_timeout(kfd_t fd, void *priv)
{
    usock_accept_t  *acpt = NULL;

    acpt = __accept_node_find(fd, priv);
    if(!acpt)
        return;

    memset(acpt->pdu, 0, sizeof(usock_pdu_t) + KYLIN_USOCK_PDU_PAYLOAD_MAX);
}

static void __server_process(kfd_t fd, void *priv)
{
    int hdr_len;
    int payload_len;
    int recv_len, send_len;
    usock_accept_t *acpt = NULL;

    hdr_len = sizeof(usock_pdu_hdr_t);

    acpt = __accept_node_find(fd, priv);
    if(!acpt)
        return;

    /*recv header of pdu*/
    do {
        recv_len = kylin_socket_recv(acpt->sock, fd, 
                acpt->pdu + acpt->pdu_pos, hdr_len - acpt->pdu_pos);
        if(recv_len == -1) {
            __server_process_error(fd, priv);
            return;
        }

        acpt->pdu_pos += recv_len;
    } while(acpt->pdu_pos < hdr_len);

    if(acpt->pdu->hdr.len >= KYLIN_USOCK_PDU_PAYLOAD_MAX) {
        __server_process_error(fd, priv);
        return;
    }

    /*recv data of pdu*/
    if(acpt->pdu->hdr.len) {
        payload_len = acpt->pdu->hdr.len + hdr_len;
        do {
            recv_len = kylin_socket_recv(acpt->sock, fd, 
                    acpt->pdu + acpt->pdu_pos, payload_len - acpt->pdu_pos);
            if(recv_len == -1) {
                __server_process_error(fd, priv);
                return;
            }

            acpt->pdu_pos += recv_len;
        } while(acpt->pdu_pos < payload_len);
    }
    else
        payload_len = hdr_len;


    acpt->pdu->hdr.err = acpt->server->proc(acpt->server, acpt->pdu->hdr.cmd, acpt->pdu->data);

    /*send header and data of pdu*/
    do {
        send_len = kylin_socket_send(acpt->sock, fd, acpt->pdu + (payload_len - acpt->pdu_pos), acpt->pdu_pos);
        if(send_len == -1) {
            __server_process_error(fd, priv);
            return;
        }

        acpt->pdu_pos -= send_len;
    } while(acpt->pdu_pos > 0);

    return;
}

kerr_t server_init(void)
{
    acpt_priv = malloc(sizeof(kusock_server_t));
    proc_priv = malloc(sizeof(usock_accept_t)); 
    if(!acpt_priv || !proc_priv)
        return KYLIN_ERROR_NOMEM;

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

    kevent_opts_t acpt_event_opts = {
        .priv   = acpt_priv,
        .action = {
            .recv    = __server_accept,
            .send    = NULL,
            .error   = NULL,
            .timeout = NULL
        }
    };

    kevent_opts_t proc_event_opts = {
        .priv   = proc_priv,
        .action = {
            .recv    = __server_process,
            .send    = NULL,
            .error   = __server_process_error,
            .timeout = __server_process_timeout
        }
    };

    accept_event = kylin_event_create(KEVENT_TYPE_EPOLL, &acpt_event_opts);
    process_event = kylin_event_create(KEVENT_TYPE_EPOLL, &proc_event_opts);
    if(!accept_event || !process_event)
        return KYLIN_ERROR_NOENT;

    serv_rb = kylin_rb_create(&usock_serv_opts);
    if(!serv_rb)
        return KYLIN_ERROR_NOMEM;

    return pthread_create(&serv_tid, NULL, __server_event_process, NULL);
}

void server_fini(void)
{
    if(serv_rb)
        kylin_rb_destroy(serv_rb);

    if(acpt_priv)
        free(acpt_priv);

    if(proc_priv)
        free(proc_priv);

    if(accept_event)
        kylin_event_destroy(accept_event);

    if(process_event)
        kylin_event_destroy(process_event);
}

