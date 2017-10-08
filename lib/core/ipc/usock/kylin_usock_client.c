#include <kylin/include/kylin.h>
#include <kylin/include/ipc/kylin_usock.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/ipc/usock/kylin_usock.h>
#include <kylin/lib/core/ipc/usock/kylin_usock_client.h>

/*存储所有已打开的客户端(kusock_client_t)信息*/
static krb_t *cli_rb = NULL;

kusock_client_t *kylin_usock_client_open(const char *name)
{
    int len;
    struct sockaddr_un remote;
    kusock_client_t *cli = NULL;

    if(strlen(name) > KYLIN_NAME_LENGTH)
        return NULL;

    cli = malloc(sizeof(kusock_client_t));
    if(!cli)
        return NULL;

    memset(cli, 0, sizeof(kusock_client_t));

    cli->sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(cli->sock == -1) {
        free(cli);
        return NULL;
    }

#ifdef SO_NOSIGPIPE
    int set = 1;
    setsockopt(cli->sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif

    memset(&remote, 0, sizeof(struct sockaddr_un));
    remote.sun_family = AF_UNIX;
    sprintf(remote.sun_path, "%s/%s", KYLIN_USOCK_LOCAL, name);
    len = sizeof(remote.sun_family ) + strlen(remote.sun_path) + 1;

    if(connect(cli->sock, (struct sockaddr *)&remote, len) == -1) {
        close(cli->sock);
        free(cli);
        return NULL;
    }

    memcpy(cli->name, name, strlen(name));
    kylin_spinlock_init(&cli->lock);

    if(!kylin_rb_insert(cli_rb, cli)) {
        close(cli->sock);
        free(cli);
        return NULL;
    }

    return cli;
}

void kylin_usock_client_close(kusock_client_t *cli)
{
    close(cli->sock);
    kylin_rb_remove(cli_rb, kylin_rb_find(cli_rb, cli));
}

static kerr_t __send_request_pdu(kfd_t fd, uint32_t cmd, void *data, int length)
{
    int len;
    usock_pdu_hdr_t hdr;

    if(length > KYLIN_USOCK_PDU_PAYLOAD_MAX)
        return KYLIN_ERROR_2BIG;

    memset(&hdr, 0, sizeof(usock_pdu_hdr_t));
    hdr.version = USOCK_PDU_VERSION;
    hdr.type    = USOCK_PDU_TYPE_REQUEST;
    hdr.len     = (data == NULL ? 0 : length);
    hdr.cmd     = cmd;
    hdr.err     = KYLIN_ERROR_OK;

    /*send pdu header*/
    len = send(fd, &hdr, sizeof(usock_pdu_hdr_t), 0);
    if(len != sizeof(usock_pdu_hdr_t)) 
        return errno;

    /*send pdu payload*/
    if(hdr.len > 0) {
        len = send(fd, data, hdr.len, 0);
        if(len != hdr.len)
            return errno;
    }

    return KYLIN_ERROR_OK;
}

static kerr_t __recv_response_pdu(kfd_t fd, void *data)
{
    int len;
    usock_pdu_hdr_t hdr;

    len = recv(fd, &hdr, sizeof(usock_pdu_hdr_t), 0);
    if(len != sizeof(usock_pdu_hdr_t)) 
        return KYLIN_ERROR_INVAL;

    if(hdr.err != KYLIN_ERROR_OK)
        return hdr.err;

    len = recv(fd, data, hdr.len, 0);
    if(len != hdr.len)
        return errno;

    return KYLIN_ERROR_OK;
}

kerr_t kylin_usock_client_request(kusock_client_t *cli, unsigned long cmd, void *data)
{
    kerr_t ret = KYLIN_ERROR_OK;
    int request_len;

#ifndef IOC_DIRMASK
#define IOC_DIRMASK 0xe0000000  /* mask for IN/OUT/VOID */
#endif

    request_len = ((cmd & ~IOC_DIRMASK) >> 16);

    kylin_spinlock_lock(&cli->lock);

    ret = __send_request_pdu(cli->sock, cmd, data, request_len); 
    if(ret != KYLIN_ERROR_OK)
        goto exit;

    ret = __recv_response_pdu(cli->sock, data);

exit:
    kylin_spinlock_unlock(&cli->lock);
    return ret;
}

static int __cli_compare(const void *c1, const void *c2)
{
    const kusock_client_t *comp1 = c1;
    const kusock_client_t *comp2 = c2;

    if(comp1->sock > comp2->sock)
        return 1;
    if(comp1->sock < comp2->sock)
        return -1;
    return 0;
}

kerr_t client_init(void)
{
    krb_opts_t usock_cli_opts = {
        .name      = "usock_client",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kusock_client_t),
        .compare   = __cli_compare,
        .allocator = {
            .val_ctor   = NULL, /*值所在的内存由调用者分配*/
            .val_dtor   = NULL,
            .node_ctor  = NULL,
            .node_dtor  = NULL,
            .guard_ctor = NULL,
            .guard_dtor = NULL
        }
    };

    cli_rb = kylin_rb_create(&usock_cli_opts);
    if(!cli_rb)
        return KYLIN_ERROR_NOMEM;

    return KYLIN_ERROR_OK;
}

void client_fini(void)
{
    if(cli_rb)
        kylin_rb_destroy(cli_rb);
}

