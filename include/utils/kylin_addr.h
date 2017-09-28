#ifndef __KYLIN_ADDR_H__
#define __KYLIN_ADDR_H__

#include <utils/kylin_base.h>

#define IPADDR_FMT  "%u.%u.%u.%u"
#define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define IP6ADDR_FMT  "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X "
#define NIP6QUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3], \
        ((unsigned char *)&addr)[4], \
        ((unsigned char *)&addr)[5], \
        ((unsigned char *)&addr)[6], \
        ((unsigned char *)&addr)[7], \
        ((unsigned char *)&addr)[8], \
        ((unsigned char *)&addr)[9], \
        ((unsigned char *)&addr)[10], \
        ((unsigned char *)&addr)[11], \
        ((unsigned char *)&addr)[12], \
        ((unsigned char *)&addr)[13], \
        ((unsigned char *)&addr)[14], \
        ((unsigned char *)&addr)[15] 


#define NETADDR_FMT "%u.%u.%u"
#define NNETQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2]

#define MACADDR_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define NMACQUAD(x) ((uint8_t*)(x))[0],((uint8_t*)(x))[1],((uint8_t*)(x))[2],((uint8_t*)(x))[3],((uint8_t*)(x))[4],((uint8_t*)(x))[5]

typedef struct kylin_addr_ipv4 {
    union {
        struct in_addr     v4;
        uint8_t            addr8[4];
        uint16_t           addr16[2];
        uint32_t           addr32[1];
    } ma;       /* 32-bit address */
} __attribute__ ((packed)) kylin_addr_ipv4_t;

typedef struct kylin_addr {
    union {
        struct in_addr      v4;
        struct in6_addr     v6;
        uint8_t             addr8[16];
        uint16_t            addr16[8];
        uint32_t            addr32[4];
        uint64_t            addr64[2];
    } ma;           /* 128-bit address */    
#define v4  ma.v4
#define v6  ma.v6
#define addr8   ma.addr8
#define addr16  ma.addr16
#define addr32  ma.addr32
#define addr64  ma.addr64
} __attribute__ ((packed)) kaddr_t;

typedef struct kylin_sockaddr 
{
    uint8_t             sa_family;
    uint8_t             sa_len;
    struct kylin_addr  sa_addr;
} __attribute__ ((packed)) kylin_sockaddr;

typedef struct kylin_in_addr 
{
    uint32_t              s_addr;
} __attribute__ ((packed)) kylin_in_addr_t;

typedef struct kylin_sockaddr_in 
{
    unsigned char sin_len;
    sa_family_t   sin_family;
    struct kylin_in_addr      sin_addr;
} __attribute__ ((packed)) kylin_sockaddr_in_t; 

static __inline int kylin_addr_comp(uint8_t af, struct kylin_addr *a, struct kylin_addr *b) {
    if (af == AF_INET) {
        if (ntohl(a->v4.s_addr) > ntohl(b->v4.s_addr))
            return (1);
        if (ntohl(a->v4.s_addr) < ntohl(b->v4.s_addr))
            return (-1);
    }
    else {
        if (ntohl(a->addr32[0]) > ntohl(b->addr32[0]))
            return (1);
        if (ntohl(a->addr32[0]) < ntohl(b->addr32[0]))
            return (-1);
        if (ntohl(a->addr32[1]) > ntohl(b->addr32[1]))
            return (1);
        if (ntohl(a->addr32[1]) < ntohl(b->addr32[1]))
            return (-1);
        if (ntohl(a->addr32[2]) > ntohl(b->addr32[2]))
            return (1);
        if (ntohl(a->addr32[2]) < ntohl(b->addr32[2]))
            return (-1);
        if (ntohl(a->addr32[3]) > ntohl(b->addr32[3]))
            return (1);
        if (ntohl(a->addr32[3]) < ntohl(b->addr32[3]))
            return (-1);
    }
    return 0;
}

static inline void kylin_addr_zero(uint8_t af, struct kylin_addr *addr)
{
    if ((af) == AF_INET) {
        (addr)->v4.s_addr = 0;
    }
    else {
        (addr)->addr32[0] = 0; 
        (addr)->addr32[1] = 0;
        (addr)->addr32[2] = 0;
        (addr)->addr32[3] = 0;
    }                                                   
}

static inline void kylin_addr_copy(uint8_t af, struct kylin_addr *dst, struct kylin_addr *src)
{
    if ((af) == AF_INET) {
        (dst)->v4.s_addr = (src)->v4.s_addr;
    }
    else {
        (dst)->addr32[0] = (src)->addr32[0]; 
        (dst)->addr32[1] = (src)->addr32[1];
        (dst)->addr32[2] = (src)->addr32[2];
        (dst)->addr32[3] = (src)->addr32[3];
    }                                                   
}
        
static inline void kylin_addr_dump(uint8_t af, struct kylin_addr *addr) 
{
    if(af == AF_INET) 
        printf( IPADDR_FMT"\n", NIPQUAD(addr->v4) ); 
     else if (af == AF_INET6) 
        printf( IP6ADDR_FMT"\n", NIP6QUAD(addr->addr8) );
}

static inline int kylin_addr_is_zero(uint8_t af, struct kylin_addr *addr)
{
    int is_zero = 0;
    
    if(af == AF_INET) {
        is_zero = (addr->v4.s_addr == 0) ? 1 : 0;
    }
    else {
        is_zero = ((addr->addr64[0] + addr->addr64[1]) == 0) ? 1 : 0;
    }

    return is_zero;
}

static inline int kylin_addr_is_max(uint8_t af, struct kylin_addr *addr)
{
    int is_max = 0;
    
    if(af == AF_INET) {
        is_max = (addr->v4.s_addr == 0xffffffff) ? 1 : 0;
    }
    else {
        if(addr->addr64[0] == 0xffffffffffffffffL &&
                addr->addr64[1] == 0xffffffffffffffffL) {
            is_max = 1;
        }
    }

    return is_max;
}


/* convert struct sockaddr to struct kylin_addr */
static inline void kylin_addr_sockaddr_2_paddr(struct sockaddr *sock_addr,
    uint8_t *af, struct kylin_addr *addr)
{
    struct sockaddr_in      *v4_addr;
    struct sockaddr_in6     *v6_addr;

    memset(addr, 0, sizeof(struct kylin_addr));
 
    v4_addr = (struct sockaddr_in *)sock_addr;
    if(v4_addr->sin_family == AF_INET) { 
        *af  = AF_INET;
        addr->v4 = v4_addr->sin_addr;
    }
    else {
        /* AF_INET6 */
        *af = AF_INET;
        v6_addr = (struct sockaddr_in6 *)sock_addr;
        memcpy(addr, &(v6_addr->sin6_addr), sizeof(struct kylin_addr)); 
    }        
}

/* use address family convert struct sockaddr to struct kylin_addr */
static inline void kylin_addr_sockaddr_2_paddr_af(struct sockaddr *sock_addr,
    uint8_t af, struct kylin_addr *addr)
{
    struct sockaddr_in      *v4_addr;
    struct sockaddr_in6     *v6_addr;

    memset(addr, 0, sizeof(struct kylin_addr));
 
    if(af == AF_INET) { 
        v4_addr = (struct sockaddr_in *)sock_addr;
        addr->v4 = v4_addr->sin_addr;
    }
    else {
        /* AF_INET6 */
        v6_addr = (struct sockaddr_in6 *)sock_addr;
        memcpy(addr, &(v6_addr->sin6_addr), sizeof(struct kylin_addr)); 
    }        
}

/* convert struct kylin_addr to struct sockaddr */
static inline void kylin_addr_paddr_2_sockaddr_af(uint8_t af, struct kylin_addr *addr, 
    struct sockaddr *sock_addr)
{
    struct sockaddr_in      *v4_addr;
    struct sockaddr_in6     *v6_addr;

    if(af == AF_INET) { 
        v4_addr = (struct sockaddr_in *)sock_addr;
        v4_addr->sin_family  = AF_INET;
        v4_addr->sin_port = 0;
        v4_addr->sin_addr = addr->v4;
    }
    else {
        /* AF_INET6 */
        v6_addr = (struct sockaddr_in6 *)sock_addr;
        memset(v6_addr, 0, sizeof(struct sockaddr_in6));

        v6_addr->sin6_family = AF_INET6;
        memcpy(&(v6_addr->sin6_addr), addr, sizeof(struct kylin_addr)); 
    }        
}

/* netmask bits (xxx/24) to netmask address (xxx/255.255.255.0) */
static inline void kylin_addr_maskbits_2_maskaddr(uint8_t af, uint8_t netmask_bits, struct kylin_addr *mask_addr)
{
    int n = 0;
    int i;
    int max;

    max = (af == AF_INET) ? 4 : 16;

    if(af == AF_INET && netmask_bits > 32) {
        netmask_bits = 32;
    }    
    else if(netmask_bits > 128) {
        netmask_bits = 128;
    }

    while(netmask_bits >= 8) {
        netmask_bits -= 8;
        mask_addr->addr8[n] = 0xff;        
        n++;
    }

    if(netmask_bits > 0) {
        mask_addr->addr8[n] = ~((1LL << (8 - netmask_bits)) - 1);
        n++;
    }

    for(i = n; i < max; i++) {
        mask_addr->addr8[i] = 0x00;
    }

    return;
}

/* netmask address (xxx/255.255.255.0) to netmask bits (xxx/24) */
static inline void kylin_addr_maskaddr_2_maskbits(uint8_t af, struct kylin_addr *mask_addr, uint8_t *netmask_bits)
{
    int i;
    int n;
    uint8_t value;
    
    *netmask_bits = 0;
    
    n = (af == AF_INET) ? 4 : 16;

    for(i = 0; i < n; i++) {
        value = mask_addr->addr8[i];
        if(value == 0xff) {
            (*netmask_bits) += 8;
        }
        else {
            while(value > 0) {
                (*netmask_bits)++;
                value = value << 1;
            }            
            break;
        }
    }
}

static inline void kylin_addr_broadaddr(uint8_t af, struct kylin_addr *addr, struct kylin_addr *mask_addr,
    struct kylin_addr *broadcast_addr)
{
    int i;
    int max;

    max = (af == AF_INET) ? 4 : 16;

    for(i = 0; i < max; i++) {
        /* broadcast = ip | ( ~ subnet ) */
        broadcast_addr->addr8[i] = addr->addr8[i] | (~(mask_addr->addr8[i]));  
    }
    
    return;
}
 
typedef struct kylin_host 
{
    struct kylin_addr     addr;   /* 128bits, 16bytes */
    uint16_t                port;   /* 2 bytes */
}__attribute__ ((packed))   kylin_host_t;

typedef struct kylin_host_tuple 
{
        uint8_t                proto;
        struct kylin_host    src; /* 18 bytes */
        struct kylin_host    dst; /* 18 bytes */
}__attribute__ ((packed))  kylin_host_tuple_t;

static inline void kylin_host_tuple_dump(uint8_t af, struct kylin_host_tuple *tuple) 
{
    if(af == AF_INET) {
        printf(IPADDR_FMT":%u -> "IPADDR_FMT":%u, proto:%u\n", 
                NIPQUAD(tuple->src.addr.v4), 
            ntohs(tuple->src.port),
            NIPQUAD(tuple->dst.addr.v4), 
            ntohs(tuple->dst.port),
            tuple->proto);            
    } else if (af == AF_INET6) {
        printf(IP6ADDR_FMT":%u -> "IP6ADDR_FMT":%u, proto:%u\n",
            NIP6QUAD(tuple->src.addr.addr8),
            ntohs(tuple->src.port),
            NIP6QUAD(tuple->dst.addr.addr8),
            ntohs(tuple->dst.port),
            tuple->proto);
    }
}

static inline void kylin_host_tuple_copy(uint8_t af, struct kylin_host_tuple *dst,
                        const struct kylin_host_tuple *src)
{
    memcpy(dst, src, sizeof(struct kylin_host_tuple));
}

static inline void kylin_host_tuple_invert(uint8_t af, struct kylin_host_tuple *tuple)
{
    uint16_t port;
    uint32_t addrx;
    struct kylin_addr addr;
 
    port = tuple->src.port;
    tuple->src.port = tuple->dst.port;
    tuple->dst.port = port;        
 
    if(af == AF_INET) {

        addrx = tuple->src.addr.addr32[0];
        tuple->src.addr.addr32[0] = tuple->dst.addr.addr32[0];
        tuple->dst.addr.addr32[0] = addrx;
    }
    else {
       
        kylin_addr_copy(af, &addr, &tuple->src.addr);
        kylin_addr_copy(af, &tuple->src.addr, &tuple->dst.addr);
        kylin_addr_copy(af, &tuple->dst.addr, &addr);
 
    }
}

static inline int kylin_host_tuple_src_equal(uint8_t af, const struct kylin_host_tuple *t1,
                        const struct kylin_host_tuple *t2)
{
    if(t1->proto == t2->proto) {
        if(t1->src.port == t2->src.port) {
            if(af == AF_INET) {
                return (t1->src.addr.addr32[0] == t2->src.addr.addr32[0]);
            }
            return (t1->src.addr.addr32[0] == t2->src.addr.addr32[0] && 
                t1->src.addr.addr32[1] == t2->src.addr.addr32[1] &&
                t1->src.addr.addr32[2] == t2->src.addr.addr32[2] &&
                t1->src.addr.addr32[3] == t2->src.addr.addr32[3]);
        }
    }
    return 0;
}

static inline int kylin_host_tuple_dst_equal(uint8_t af, const struct kylin_host_tuple *t1,
                        const struct kylin_host_tuple *t2)
{
    if(t1->proto == t2->proto) {
        if(t1->dst.port == t2->dst.port) {
            if(af == AF_INET) {
                return (t1->dst.addr.addr32[0] == t2->dst.addr.addr32[0]);
            }
            return (t1->dst.addr.addr32[0] == t2->dst.addr.addr32[0] && 
                t1->dst.addr.addr32[1] == t2->dst.addr.addr32[1] &&
                t1->dst.addr.addr32[2] == t2->dst.addr.addr32[2] &&
                t1->dst.addr.addr32[3] == t2->dst.addr.addr32[3]);
        }
    }
    return 0;
}

static inline int kylin_host_tuple_equal(uint8_t af, const struct kylin_host_tuple *t1,
                        const struct kylin_host_tuple *t2)
{
    return kylin_host_tuple_src_equal(af, t1, t2) && kylin_host_tuple_dst_equal(af, t1, t2);
}

// is equel, return 1, else return 0
static inline int kylin_host_tuple_mask_cmp(uint8_t af, struct kylin_host_tuple *t,
                       struct kylin_host_tuple *tuple,
                       struct kylin_host_tuple *mask)
{
    if(!(((t->proto ^ tuple->proto) & mask->proto)
        ||((t->src.port ^ tuple->src.port) & mask->src.port)
        || ((t->dst.port ^ tuple->dst.port) & mask->dst.port))) {
        if(af == AF_INET) {
            return !(((t->src.addr.addr32[0] ^ tuple->src.addr.addr32[0]) & mask->src.addr.addr32[0]) 
                        || ((t->dst.addr.addr32[0] ^ tuple->dst.addr.addr32[0]) & mask->dst.addr.addr32[0]));
        }
        return !(((t->src.addr.addr32[0] ^ tuple->src.addr.addr32[0]) & mask->src.addr.addr32[0]) 
                    || ((t->dst.addr.addr32[0] ^ tuple->dst.addr.addr32[0]) & mask->dst.addr.addr32[0])
                    || ((t->src.addr.addr32[1] ^ tuple->src.addr.addr32[1]) & mask->src.addr.addr32[1]) 
                    || ((t->dst.addr.addr32[1] ^ tuple->dst.addr.addr32[1]) & mask->dst.addr.addr32[1])
                    || ((t->src.addr.addr32[2] ^ tuple->src.addr.addr32[2]) & mask->src.addr.addr32[2]) 
                    || ((t->dst.addr.addr32[2] ^ tuple->dst.addr.addr32[2]) & mask->dst.addr.addr32[2])
                    || ((t->src.addr.addr32[3] ^ tuple->src.addr.addr32[3]) & mask->src.addr.addr32[3]) 
                    || ((t->dst.addr.addr32[3] ^ tuple->dst.addr.addr32[3]) & mask->dst.addr.addr32[3]));        
    }
    return 0;
}

#define KYLIN_ADDR_MAC_ADDR_STR_LEN 18
#define KYLIN_ADDR_MAC_ADDR_HEX_LEN 6

/*f0:79:59:69:b6:70  -> f0795969b670*/
static inline int kylin_addr_mac_addr_str2hex(int strlen, char *str, int hexlen, unsigned char *hex) 
{
    unsigned char *start, *stop;
    int pos = 0;
    
    memset(hex, 0, hexlen);
    if (strlen < (KYLIN_ADDR_MAC_ADDR_HEX_LEN << 1) - 1) {
        return 0;
    }

    start = (unsigned char *)str;
    stop = (unsigned char *)str + strlen - 1;
    while (start <= stop) {
        unsigned char *ch;

        ch = kylin_strchr(start, stop, ':'); 
        if (ch == NULL) {
            if (pos != KYLIN_ADDR_MAC_ADDR_HEX_LEN - 1)
                return -EINVAL;
            ch = stop + 1;
        }

        if ((ch - start) == 2) {
            if (*start >= '0' && *start <= '9') {
                hex[pos] = (*start - '0') << 4;
            } else if (*start >= 'A' && *start <= 'F') {
                hex[pos] = (*start - 'A' + 10) << 4;
            } else if (*start >= 'a' && *start <= 'f') {
                hex[pos] = (*start - 'a' + 10) << 4;
            }

            if (*(start + 1) >= '0' && *(start + 1) <= '9') {
                hex[pos] |= (*(start + 1) - '0');
            } else if (*(start + 1) >= 'A' && *(start + 1) <= 'F') {
                hex[pos] |= (*(start + 1) - 'A' + 10);
            } else if (*(start + 1) >= 'a' && *(start + 1) <= 'f') {
                hex[pos] |= (*(start + 1) - 'a' + 10);
            }
            pos++;
        } else if ((ch - start) == 1) {
            if (*start >= '0' && *start <= '9') {
                hex[pos] = (*start - '0');
            } else if (*start >= 'A' && *start <= 'F') {
                hex[pos] = (*start - 'A' + 10);
            } else if (*start >= 'a' && *start <= 'f') {
                hex[pos] = (*start - 'a' + 10);
            }
            pos++;
        } else {
            return -EINVAL;
        }

        if (pos > hexlen) {
            return -EINVAL;
        }
        start = ch + 1;
    }

    return 0;
}

#endif /* __KYLIN_ADDR_H__ */

