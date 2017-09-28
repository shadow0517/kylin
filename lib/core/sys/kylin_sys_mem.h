#ifndef _KYLIN_SYS_COMMON_MEM_H_
#define _KYLIN_SYS_COMMON_MEM_H_

#include <kylin/include/sys/kylin_sys_mem.h>

/*
 * HugePage Memory configure in system
 */
typedef struct {
    ksys_hp_t               hp_size; /*size of a huge page*/
    const char             *hugedir; /*dir where hugetlbfs is mounted*/
    /*number of hugepages of this size on each socket*/
    uint32_t                page_nums[KYLIN_MAX_NUMA_NODES];     
    int                     lock_desc; /*file descriptor for hugepage dir*/
} ksys_hp_t;

#endif /*_KYLIN_SYS_COMMON_MEM_H_*/
