#ifndef _KYLIN_CORE_IPC_UNIXSOCK_H_
#define _KYLIN_CORE_IPC_UNIXSOCK_H_

#include <kylin/include/kylin.h>
#include <kylin/include/ipc/kylin_usock.h>

#define KYLIN_USOCK_LOCAL  "/var/ipc/usock/"

#define USOCK_PDU_VERSION        0x01

#define USOCK_PDU_TYPE_NONE      0x00
#define USOCK_PDU_TYPE_REQUEST   0x01
#define USOCK_PDU_TYPE_RESPONSE  0x02
#define USOCK_PDU_TYPE_MAX       0x02

typedef struct {
    uint8_t     version;
    uint8_t     type;
    uint16_t    len;
    uint32_t    cmd;
    int         err;
} __kylin_packed usock_pdu_hdr_t;

typedef struct {
    usock_pdu_hdr_t  head;
    uint8_t          data[];
} __kylin_packed usock_pdu_t;

extern kerr_t kylin_usock_init(void);
extern void kylin_usock_fini(void);

#endif /*_KYLIN_CORE_IPC_UNIXSOCK_H_*/
