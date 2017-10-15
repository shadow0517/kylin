#include <kylin/include/kylin.h>
#include <kylin/include/ipc/kylin_usock.h>
#include <kylin/include/math/kylin_rb.h>

#include <kylin/lib/core/ipc/usock/kylin_usock.h>
#include <kylin/lib/core/ipc/usock/kylin_usock_client.h>

/*存储所有已打开的客户端(kusock_client_t)信息*/
static krb_t *cli_rb = NULL;

kusock_client_t *kylin_usock_client_open(const char *name)
{
    struct sockaddr_un remote;
    kusock_client_t *cli = NULL;

    if(strlen(name) > KYLIN_NAME_LENGTH)
        return NULL;

    remote.sun_family = AF_UNIX;
    sprintf(remote.sun_path, "%s/%s", KYLIN_USOCK_LOCAL, name);

    ksock_opts_t sock_opts = {
        .type   = SOCK_STREAM,
        .config = {
            .client = {
                .remote = {
                    .len  = sizeof(remote.sun_family) + strlen(remote.sun_path) + 1,
                    .addr = {
                        .un = remote
                    }
                }
            }
        }
    };

    cli = malloc(sizeof(kusock_client_t));
    if(!cli)
        return NULL;

    memset(cli, 0, sizeof(kusock_client_t));

    cli->sock = kylin_socket_create(KYLIN_SOCK_CLIENT_UNIX, &sock_opts);
    if(!cli->sock) 
        goto error;

    cli->fd = kylin_socket_get_fd(cli->sock);
    if(cli->fd == -1) 
        goto error;

    if(kylin_socket_connect(cli->sock) != KYLIN_ERROR_OK) 
        goto error;

    memcpy(cli->name, name, strlen(name));
    kylin_spinlock_init(&cli->lock);

    if(!kylin_rb_insert(cli_rb, cli)) 
        goto error;

    return cli;

error:
    if(cli && cli->sock)
        kylin_socket_destroy(cli->sock);

    if(cli)
        free(cli);

    return NULL;
}

void kylin_usock_client_close(kusock_client_t *cli)
{
    kylin_socket_destroy(cli->sock);
    kylin_rb_remove(cli_rb, cli);
}

static kerr_t __send_request_pdu(kusock_client_t *cli, uint32_t cmd, void *data, int length)
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
    len = kylin_socket_send(cli->sock, cli->fd, &hdr, sizeof(usock_pdu_hdr_t));
    if(len != sizeof(usock_pdu_hdr_t)) 
        return errno;

    /*send pdu payload*/
    if(hdr.len > 0) {
        len = kylin_socket_send(cli->sock, cli->fd, data, hdr.len);
        if(len != hdr.len)
            return errno;
    }

    return KYLIN_ERROR_OK;
}

static kerr_t __recv_response_pdu(kusock_client_t *cli, void *data)
{
    int len;
    usock_pdu_hdr_t hdr;

    len = kylin_socket_recv(cli->sock, cli->fd, &hdr, sizeof(usock_pdu_hdr_t));
    if(len != sizeof(usock_pdu_hdr_t)) 
        return KYLIN_ERROR_INVAL;

    if(hdr.err != KYLIN_ERROR_OK)
        return hdr.err;

    len = kylin_socket_recv(cli->sock, cli->fd, data, hdr.len);
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

    ret = __send_request_pdu(cli, cmd, data, request_len); 
    if(ret != KYLIN_ERROR_OK)
        goto exit;

    ret = __recv_response_pdu(cli, data);

exit:
    kylin_spinlock_unlock(&cli->lock);
    return ret;
}

static int __cli_compare(const void *c1, const void *c2)
{
    const kusock_client_t *comp1 = c1;
    const kusock_client_t *comp2 = c2;

    if(comp1->fd > comp2->fd )
        return 1;
    if(comp1->fd < comp2->fd)
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
        .allocator = KRB_OPTS_ALLOCATOR_NULL
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

