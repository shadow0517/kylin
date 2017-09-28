#ifndef _KYLIN_SYS_COMMON_SYSTEM_H_
#define _KYLIN_SYS_COMMON_SYSTEM_H_

#include <kylin/include/sys/kylin_system.h>

struct kylin_system {
    size_t           total_mem;
    uint8_t          socket_num;
    size_t           socket_mem[KYLIN_MAX_NUMA_NODES]; /*amount of memory per socket*/

    const char      *hugefile_prefix; /*the base filename of hugetlbfs files*/
    const char      *hugepage_dir; /*specific hugetlbfs directory to use*/

    uint8_t          hugepage_num;
    ksys_mem_t       hugepage[KYLIN_HUGE_MAX];

    ksys_node_t      node[KYLIN_MAX_NUMA_NODES];
};

#endif /*_KYLIN_SYS_COMMON_SYSTEM_H_*/
