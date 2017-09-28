#ifndef _KYLIN_SYS_NIC_H_
#define _KYLIN_SYS_NIC_H_

/*Network Interface Card info*/
typedef struct {
    katomic64_t rx_bytes;
    katomic64_t rx_packets;
    katomic64_t tx_bytes;
    katomic64_t tx_packets;
    katomic64_t rx_dropped;
    katomic64_t tx_dropped;
    katomic64_t rx_errors;
    katomic64_t tx_errors;
    katomic64_t rx_fifo_errors;
    katomic64_t tx_fifo_errors;
    katomic64_t rx_compressed;
    katomic64_t tx_compressed;
} ksys_nic_statistics_t;

typedef enum {
    KYLIN_NIC_ENCAP_ETHER;

} ksys_nic_encap_t;

/*
 * Network Interface Card info Per <NUMA or UMA> Node
 *
 * It's Running Time Environment Variables, Not Configure
 * */
typedef struct {
    uint32_t                id;
    ksys_nic_encap_t        encap; /*ethernet, local loopback*/
    uint8_t                 link;
    uint32_t                mtu;

    kstr_t                  name;

    kmac_t                  mac;
    kip_t                   ip;
    kip_t                   mask;
    kip6_t                  ip6;
    kip6_t                  mask6;

    ksys_nic_statistics_t   statistics;
} ksys_nic_t;

#endif /*_KYLIN_SYS_NIC_H_*/
