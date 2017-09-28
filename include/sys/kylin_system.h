#ifndef _KYLIN_SYS_SYSTEM_H_
#define _KYLIN_SYS_SYSTEM_H_

#include <kylin/include/arch/kylin_atomic.h>
#include <kylin/include/arch/kylin_cpuflag.h>
#include <kylin/include/utils/kylin_addr.h>

#include <kylin/include/sys/kylin_sys_lcore.h>
#include <kylin/include/sys/kylin_sys_mem.h>
#include <kylin/include/sys/kylin_sys_nic.h>

/*system info should be shared between multi-process*/

#define KYLIN_CPUFLAG_NUM_MAX  (KYLIN_CPUFLAG_NUMFLAGS + 1)
#define KYLIN_MAX_NUMA_NODES         8
#define KYLIN_MAX_LCORE_PER_NODE     32
#define KYLIN_MAX_NIC_PER_NODE       32

struct kylin_system;
struct kylin_system_node;
typedef struct kylin_system ksys_t;
typedef struct kylin_system_node ksys_node_t;

/*Node info*/
struct kylin_system_node{
    uint8_t                id;
    uint8_t                lcore_count;
    uint8_t                nic_count;

    ksys_lcore_t           lcore[KYLIN_MAX_LCORE_PER_NODE];
    ksys_nic_t             nic[KYLIN_MAX_NIC_PER_NODE];
};

typedef struct {


} ksys_hpet_t;

typedef struct {

} ksys_tsc_t;


#endif /*_KYLIN_SYS_SYSTEM_H_*/
