#ifndef _KYLIN_SOCKET_H_
#define _KYLIN_SOCKET_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

struct kylin_socket;
typedef struct kylin_socket ksock_t;

typedef enum {
    KYLIN_SOCK_SERVER_TCP   = 0x00,
    KYLIN_SOCK_SERVER_UDP   = 0x01,
    KYLIN_SOCK_SERVER_UNIX  = 0x02,
    KYLIN_SOCK_SERVER_MAX   = 0x0F, /*允许注册最多15个server套接字类型*/
    KYLIN_SOCK_CLIENT_TCP   = 0x10,
    KYLIN_SOCK_CLIENT_UDP   = 0x11,
    KYLIN_SOCK_CLIENT_UNIX  = 0x12,
    KYLIN_SOCK_CLIENT_MAX   = 0x1F, /*允许注册最多15个client套接字类型*/
    KYLIN_SOCK_MAX          = 0x20
} ksock_type_t;

typedef struct {
    socklen_t len;
    union {
        struct sockaddr_in in;
        struct sockaddr_un un;
    } addr;
} ksock_addr_t;

/*对于流类型的套接字，该结构表示一个连接*/
typedef struct {
    kfd_t         fd;
    ksock_t      *sock;    /*server socket*/
    ksock_addr_t  server; 
    ksock_addr_t  client;  
} ksock_conn_t;

typedef struct {
    int type; /*SOCK_STREAM, SOCK_DGRAM and so on, unixsock should set*/
    union {
        struct {
            int          backlog;
            ksock_addr_t local;
        } server;
        struct {
            ksock_addr_t remote;
        } client;
    } config;
} ksock_opts_t;

extern ksock_t *kylin_socket_create(ksock_type_t, const ksock_opts_t *);
extern void kylin_socket_destroy(ksock_t *);

extern kfd_t kylin_socket_get_fd(ksock_t *);

extern ksock_conn_t *kylin_socket_accept(ksock_t *);

extern void kylin_socket_connection_destroy(ksock_t *, ksock_conn_t *);

extern kerr_t kylin_socket_connect(ksock_t *);

extern ssize_t kylin_socket_recv(ksock_t *, kfd_t, void *, size_t);
extern ssize_t kylin_socket_send(ksock_t *, kfd_t, const void *, size_t);

extern void *kylin_socket_get_priv(ksock_t *);
extern ksock_type_t kylin_socket_get_type(ksock_t *); 
extern ksock_opts_t *kylin_socket_get_opts(ksock_t *);

typedef struct {
    void *(*create)(ksock_t *);
    void  (*destroy)(void *);

    kerr_t (*connect)(ksock_t *); /*client*/
    ksock_conn_t *(*accept)(ksock_t *);  /*server*/

    /*TODO:对于数据报套接字，调用这两个函数会造成数据错乱*/
    ssize_t (*recv)(ksock_t *, kfd_t, void *, size_t);
    ssize_t (*send)(ksock_t *, kfd_t, const void *, size_t);

    /*connection api*/
    uint32_t (*conn_count)(ksock_t *);
    ksock_conn_t *(*conn_get_first)(ksock_t *);
    ksock_conn_t *(*conn_get_next)(ksock_t *, ksock_conn_t *);
    void (*conn_destroy)(ksock_t *, ksock_conn_t *);

    kfd_t (*get_sockfd)(ksock_t *);

    kerr_t (*init)(void);
    void (*fini)(void);
} ksock_reg_t;

extern int kylin_socket_register(ksock_type_t, ksock_reg_t *);
extern void kylin_socket_unregister(ksock_type_t);

#endif /*_KYLIN_SOCKET_H_*/
